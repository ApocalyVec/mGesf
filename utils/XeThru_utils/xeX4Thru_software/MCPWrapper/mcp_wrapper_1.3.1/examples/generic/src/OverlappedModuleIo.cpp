#include "OverlappedModuleIo.hpp"
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>

static std::string GetLastErrorString()
{
    DWORD error = GetLastError();
    if (error) {
        LPVOID lpMessageBuffer;
        DWORD messageLength = FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    error,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) &lpMessageBuffer,
                    0, NULL);
        if (messageLength > 0) {
            LPCSTR lpMessageString = (LPCSTR)lpMessageBuffer;
            std::string result(lpMessageString, lpMessageString + messageLength);
            LocalFree(lpMessageBuffer);
            return result;
        }
    }
    return std::string();
}


OverlappedModuleIo::OverlappedModuleIo():
    hSerial(INVALID_HANDLE_VALUE),
	baudrate_(CBR_115200)
{
    //open(device_name);
}


int OverlappedModuleIo::open(const std::string & device_name)
{
    // Open the highest available serial port number
    const std::string device_prefix = "\\\\.\\";
    const std::string device_file = device_prefix + device_name;
    hSerial = CreateFile(
        device_file.c_str(),
        GENERIC_READ|GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL );


    if (hSerial == INVALID_HANDLE_VALUE){
	//throw std::runtime_error("Unable to create com port handle");
        return GetLastError();
    }

    // remove all data in buffers
    PurgeComm(hSerial, PURGE_TXCLEAR | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_RXABORT);

    reconfigurePort();
    return 0;
}

void OverlappedModuleIo::setBaudrate(unsigned int baudrate)
{
	baudrate_ = baudrate;
	reconfigurePort();
}

void OverlappedModuleIo::reconfigurePort()
{
    // Set device parameters (38400 baud, 1 start bit,
    // 1 stop bit, no parity)
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
    {
        CloseHandle(hSerial);
        throw std::runtime_error("Unable to set serial params");
    }

	dcbSerialParams.BaudRate = baudrate_; //CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;
    if(SetCommState(hSerial, &dcbSerialParams) == 0)
    {
        CloseHandle(hSerial);
        throw std::string("Unable to set serial devicde parameters");
    }

    setTimeout(100);
}

OverlappedModuleIo::~OverlappedModuleIo()
{
    if (hSerial == INVALID_HANDLE_VALUE)
        return;

    CloseHandle(hSerial);
}

int OverlappedModuleIo::write(
        const unsigned char * data,
        unsigned int length)
{
    OVERLAPPED writerOverlapped = {0};
    writerOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    DWORD bytes_written = 0;
    DWORD ok = WriteFile(hSerial, data, length, &bytes_written, &writerOverlapped);
    if (ok) {
        CloseHandle(writerOverlapped.hEvent);
        return bytes_written;
    }

    if (GetLastError() != ERROR_IO_PENDING)
    {
        CloseHandle(writerOverlapped.hEvent);
        CloseHandle(hSerial);
        return -1;
    }

    // Write operation has been scheduled and will complete in the future
    bytes_written = 0;
    ok = GetOverlappedResult(hSerial, &writerOverlapped, &bytes_written, true);
    if (!ok) {
        CloseHandle(writerOverlapped.hEvent);
        CloseHandle(hSerial);
        return -1;
    }

    CloseHandle(writerOverlapped.hEvent);
    return bytes_written;
}

void OverlappedModuleIo::setTimeout(unsigned int ms)
{
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadTotalTimeoutConstant = ms;
    timeouts.WriteTotalTimeoutConstant = 0;
    if(SetCommTimeouts(hSerial, &timeouts) == 0)
    {
        CloseHandle(hSerial);
        throw std::runtime_error("Unable to set serial timeouts");
    }
}


int OverlappedModuleIo::do_single_read(unsigned char * destination, unsigned int length)
{
    OVERLAPPED readerOverlapped = {0};

    readerOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!readerOverlapped.hEvent) {
        throw std::runtime_error("Failed to create reader event");
    }

    DWORD read_count = 0;
    int ok = ReadFile(hSerial, destination, length, &read_count, &readerOverlapped);
    if (ok) {
        std::vector<unsigned char> bytes(destination, destination + read_count);
        CloseHandle(readerOverlapped.hEvent);
        return read_count;
    }

    if (GetLastError() != ERROR_IO_PENDING) {
        std::cout << "ReadFile returned error: " << GetLastErrorString() << std::endl;
        CloseHandle(readerOverlapped.hEvent);
        return -1;
    }

    ok = GetOverlappedResult(hSerial, &readerOverlapped, &read_count, true);
    if (!ok) {
        CloseHandle(readerOverlapped.hEvent);
        return -1;
    }

    std::vector<unsigned char> bytes(destination, destination + read_count);
    CloseHandle(readerOverlapped.hEvent);
    return read_count;
}


unsigned int OverlappedModuleIo::read(
    unsigned char * destination,
    unsigned int max_length,
    const unsigned int millis)
{
    if (max_length == 0)
        return 0;

    int count = do_single_read(destination, 1);
    if (count < 0) {
        return count;
    }

    if (count == 0) {
        //logger.slog(5) << "timeout single byte" << done;
        return count;
    }

    ++destination;
    --max_length;

    DWORD flags = 0;
    COMSTAT comstat = {0};
    ClearCommError(hSerial, &flags, &comstat);

    if(comstat.cbInQue == 0) {
	return count;
    }

    const unsigned int bytes_to_read = comstat.cbInQue < max_length ? comstat.cbInQue : max_length;

    const unsigned int result = do_single_read(destination, bytes_to_read);

    if (result == 0) {
    }

    if (result < 0)
	return result;

    count += result;

    return count;
}
