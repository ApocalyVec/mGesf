import struct
import sys
import math

import numpy as np


#
# TODO 1: (NOW FIXED) Find the first occurrence of magic and start from there
# TODO 2: Warn if we cannot parse a specific section and try to recover
# TODO 3: Remove error at end of file if we have only fragment of TLV
#

def tlvHeaderDecode(data):
    tlvType, tlvLength = struct.unpack('2I', data)
    return tlvType, tlvLength


def parseDetectedObjects(data, numObj, tlvLength):
    detectedPoints = struct.unpack(str(numObj * 4) + 'f', data[:tlvLength])
    detectedPoints = np.asarray(detectedPoints).reshape(numObj, 4)
    # print(detectedPoints)
    return detectedPoints


def parseRangeProfile(data, tlvLength):
    for i in range(256):
        rangeProfile = struct.unpack('H', data[2 * i:2 * i + 2])
    #     print("\tRangeProf[%d]:\t%07.3f " % (i, rangeProfile[0] * 1.0 * 6 / 8 / (1 << 8)))
    # print("\tTLVType:\t%d " % (2))


def parseStats(data, tlvLength):
    interProcess, transmitOut, frameMargin, chirpMargin, activeCPULoad, interCPULoad = struct.unpack('6I', data[:24])
    # print("\tOutputMsgStats:\t%d " % (6))
    # print("\t\tChirpMargin:\t%d " % (chirpMargin))
    # print("\t\tFrameMargin:\t%d " % (frameMargin))
    # print("\t\tInterCPULoad:\t%d " % (interCPULoad))
    # print("\t\tActiveCPULoad:\t%d " % (activeCPULoad))
    # print("\t\tTransmitOut:\t%d " % (transmitOut))
    # print("\t\tInterprocess:\t%d " % (interProcess))




def tlvHeader(in_data):
    """

    :param in_data:
    :return: if no detected point at this frame, the detected point will be an empty a
    """
    magic = b'\x02\x01\x04\x03\x06\x05\x08\x07'
    headerLength = 36

    # print('Current data len is: ' + str(len(in_data)))
    offset = in_data.find(magic)
    data = in_data[offset:]
    if len(data) < headerLength:
        return False, None, None
    try:
        magic, version, length, platform, frameNum, cpuCycles, numObj, numTLVs = struct.unpack('Q7I',
                                                                                               data[:headerLength])
    except struct.error:
        # print ("Improper TLV structure found: ", (data,))
        return False, None, None
    # print("Packet ID:\t%d "%(frameNum))
    # print("Version:\t%x "%(version))
    # print("Data Len:\t\t%d", length)
    # print("TLV:\t\t%d "%(numTLVs))
    # print("Detect Obj:\t%d "%(numObj))
    # print("Platform:\t%X "%(platform))
    if version > 0x01000005 and len(data) >= length:
        try:
            subFrameNum = struct.unpack('I', data[36:40])[0]
            headerLength = 40
            # print("Subframe:\t%d "%(subFrameNum))
            pendingBytes = length - headerLength
            data = data[headerLength:]

            for i in range(numTLVs):
                tlvType, tlvLength = tlvHeaderDecode(data[:8])
                data = data[8:]
                if tlvType == 1:
                    # print('Outputting Points')
                    detected_points = parseDetectedObjects(data, numObj,
                                                           tlvLength)  # if no detected points, tlvType won't have 1
                elif tlvType == 2:
                    parseRangeProfile(data, tlvLength)
                elif tlvType == 6:
                    parseStats(data, tlvLength)
                else:
                    # print("Unidentified tlv type %d" % tlvType, 'Its len is ' + str(tlvLength))
                    pass
                data = data[tlvLength:]
                pendingBytes -= (8 + tlvLength)
            data = data[pendingBytes:]  # data that are left
            return True, data, detected_points
        except struct.error:
            # print('Packet is not complete yet')
            pass

    return False, None, None


if __name__ == "__main__":
    magic = b'\x02\x01\x04\x03\x06\x05\x08\x07'
    fileName = 'D:/PycharmProjects/mmWave_gesture_iwr6843/test_data2.dat'
    rawDataFile = open(fileName, "rb")
    rawData = rawDataFile.read()
    rawDataFile.close()
    offset = rawData.find(magic)
    rawData = rawData[offset:]

    # for i in range(len(rawData/36))
    #
    # for length, frameNum in tlvHeader(rawData):
    #     print
