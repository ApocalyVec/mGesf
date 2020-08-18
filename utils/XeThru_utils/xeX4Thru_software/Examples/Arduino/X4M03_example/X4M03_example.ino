// Written by: Øyvind Nydal Dahl
// Company: Novelda (XeThru)
// August 2018



// SERIAL PORTS:
// These definitions work for Arduino Mega, but must be changed for other Arduinos.
//
#define SerialRadar Serial1    // Used for communication with the radar
#define SerialDebug Serial    // Used for printing debug information

// Pin definitions
#define RESET_PIN 2
#define RADAR_RX_PIN 18


//
// The following values can be found in XeThru Module Communication Protocol:
//  https://www.xethru.com/community/resources/xethru-module-communication-protocol.130/
//
#define XT_START 0x7d
#define XT_STOP 0x7e
#define XT_ESCAPE 0x7f

#define XTS_FLAGSEQUENCE_START_NOESCAPE (uint32_t)7C7C7C7C

#define XTS_SPC_X4DRIVER  0x50
#define XTS_SPCX_SET      0x10
#define XTS_SPCX_GET      0x11

#define XTS_SPCXI_FPS             (uint32_t)0x00000010
#define XTS_SPCXI_PULSESPERSTEP   (uint32_t)0x00000011
#define XTS_SPCXI_ITERATIONS      (uint32_t)0x00000012
#define XTS_SPCXI_DOWNCONVERSION  (uint32_t)0x00000013
#define XTS_SPCXI_FRAMEAREA       (uint32_t)0x00000014
#define XTS_SPCXI_DACSTEP         (uint32_t)0x00000015
#define XTS_SPCXI_DACMIN          (uint32_t)0x00000016
#define XTS_SPCXI_DACMAX          (uint32_t)0x00000017
#define XTS_SPCXI_FRAMEAREAOFFSET (uint32_t)0x00000018
#define XTS_SPCXI_PRFDIV          (uint32_t)0x00000025


#define XTS_SPR_APPDATA 0x50
#define XTS_SPR_SYSTEM 0x30

#define XTS_SPR_ACK 0x10
#define XTS_SPR_ERROR 0x20

#define XTS_SPRS_BOOTING  (uint32_t)0x00000010
#define XTS_SPRS_READY    (uint32_t)0x00000011

#define RECV_BUFFER_SIZE  1000

unsigned char send_buf[64];  // Buffer for sending data to radar. Size picked at random
unsigned char recv_buf[RECV_BUFFER_SIZE];  // Buffer for receiving data from radar. Size picked at random



void setup() 
{
  // Getting the startup sequence right:
  //
  // If the RX-pin of the radar is low during reset or power-up, it goes into bootloader mode.
  // We don't want that, that's why we first set the RADAR_RX_PIN to high, then reset the module.
  pinMode(RADAR_RX_PIN, OUTPUT);
  digitalWrite(RADAR_RX_PIN, HIGH);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  delay(100);
  digitalWrite(RESET_PIN, HIGH);
  
  // Set up serial communication
  SerialRadar.begin(115200);
  SerialDebug.begin(115200);

  // After the module resets, the XTS_SPRS_BOOTING message is sent. Then, after the 
  // module booting sequence is completed and the module is ready to accept further
  // commands, the XTS_SPRS_READY command is issued. Let's wait for this.
  wait_for_ready_message();

  // The X4M03 module only supports X4Driver Level commands.
  set_pulses_per_step(10);
  set_downconversion(false);
  set_iterations(15);
  set_dac_min(949);
  set_dac_max(1100);
  set_dac_step(1);
  set_prf_div(16);
  set_frame_offset(0.2);
  set_frame_area(0.0, 1.0);

  
  
  // Setting the fps to something higher than 0 will make the radar start sending data
  set_fps(1);  

}


void loop() {
  receive_float_data_msg();
}





// Set FPS
void set_fps(float fps) 
{
  //Fill send buffer
  send_buf[0] = XT_START;
  send_buf[1] = XTS_SPC_X4DRIVER;
  send_buf[2] = XTS_SPCX_SET;
  send_buf[3] = XTS_SPCXI_FPS & 0xff;
  send_buf[4] = (XTS_SPCXI_FPS >> 8) & 0xff;
  send_buf[5] = (XTS_SPCXI_FPS >> 16) & 0xff;  
  send_buf[6] = (XTS_SPCXI_FPS >> 24) & 0xff;

  // Copy the bytes of the floats to send buffer
  memcpy(send_buf+7, &fps, 4);
  
  //Send the command
  send_command(11);
  
  // Get ACK response from radar
  get_ack();
}


void set_pulses_per_step(uint32_t pps)
{
    //Fill send buffer
  send_buf[0] = XT_START;
  send_buf[1] = XTS_SPC_X4DRIVER;
  send_buf[2] = XTS_SPCX_SET;
  send_buf[3] = XTS_SPCXI_PULSESPERSTEP & 0xff;
  send_buf[4] = (XTS_SPCXI_PULSESPERSTEP >> 8) & 0xff;
  send_buf[5] = (XTS_SPCXI_PULSESPERSTEP >> 16) & 0xff;  
  send_buf[6] = (XTS_SPCXI_PULSESPERSTEP >> 24) & 0xff;
  send_buf[7] = pps & 0xff;
  send_buf[8] = (pps >> 8) & 0xff;
  send_buf[9] = (pps >> 16) & 0xff;  
  send_buf[10] = (pps >> 24) & 0xff;

  //Send the command
  send_command(11);
  
  // Get ACK response from radar
  get_ack();
}



void set_iterations(uint32_t iterations)
{
    //Fill send buffer
  send_buf[0] = XT_START;
  send_buf[1] = XTS_SPC_X4DRIVER;
  send_buf[2] = XTS_SPCX_SET;
  send_buf[3] = XTS_SPCXI_ITERATIONS & 0xff;
  send_buf[4] = (XTS_SPCXI_ITERATIONS >> 8) & 0xff;
  send_buf[5] = (XTS_SPCXI_ITERATIONS >> 16) & 0xff;  
  send_buf[6] = (XTS_SPCXI_ITERATIONS >> 24) & 0xff;
  send_buf[7] = iterations & 0xff;
  send_buf[8] = (iterations >> 8) & 0xff;
  send_buf[9] = (iterations >> 16) & 0xff;  
  send_buf[10] = (iterations >> 24) & 0xff;

  //Send the command
  send_command(11);
  
  // Get ACK response from radar
  get_ack();
}



void set_downconversion(bool baseband)
{
    //Fill send buffer
  send_buf[0] = XT_START;
  send_buf[1] = XTS_SPC_X4DRIVER;
  send_buf[2] = XTS_SPCX_SET;
  send_buf[3] = XTS_SPCXI_DOWNCONVERSION & 0xff;
  send_buf[4] = (XTS_SPCXI_DOWNCONVERSION >> 8) & 0xff;
  send_buf[5] = (XTS_SPCXI_DOWNCONVERSION >> 16) & 0xff;  
  send_buf[6] = (XTS_SPCXI_DOWNCONVERSION >> 24) & 0xff;

  if (baseband)
    send_buf[7] = 0x01;
  else
    send_buf[7] = 0x00;

  //Send the command
  send_command(8);
  
  // Get ACK response from radar
  get_ack();
}


void set_dac_step(uint8_t step)
{
  //Fill send buffer
  send_buf[0] = XT_START;
  send_buf[1] = XTS_SPC_X4DRIVER;
  send_buf[2] = XTS_SPCX_SET;
  send_buf[3] = XTS_SPCXI_DACSTEP & 0xff;
  send_buf[4] = (XTS_SPCXI_DACSTEP >> 8) & 0xff;
  send_buf[5] = (XTS_SPCXI_DACSTEP >> 16) & 0xff;  
  send_buf[6] = (XTS_SPCXI_DACSTEP >> 24) & 0xff;
  send_buf[7] = step;

  //Send the command
  send_command(8);
  
  // Get ACK response from radar
  get_ack();
}

void set_dac_min(uint32_t dac_min)
{
  //Fill send buffer
  send_buf[0] = XT_START;
  send_buf[1] = XTS_SPC_X4DRIVER;
  send_buf[2] = XTS_SPCX_SET;
  send_buf[3] = XTS_SPCXI_DACMIN & 0xff;
  send_buf[4] = (XTS_SPCXI_DACMIN >> 8) & 0xff;
  send_buf[5] = (XTS_SPCXI_DACMIN >> 16) & 0xff;  
  send_buf[6] = (XTS_SPCXI_DACMIN >> 24) & 0xff;
  send_buf[7] = dac_min & 0xff;
  send_buf[8] = (dac_min >> 8) & 0xff;
  send_buf[9] = (dac_min >> 16) & 0xff;  
  send_buf[10] = (dac_min >> 24) & 0xff;

  //Send the command
  send_command(11);
  
  // Get ACK response from radar
  get_ack();
}


void set_dac_max(uint32_t dac_max)
{
  //Fill send buffer
  send_buf[0] = XT_START;
  send_buf[1] = XTS_SPC_X4DRIVER;
  send_buf[2] = XTS_SPCX_SET;
  send_buf[3] = XTS_SPCXI_DACMAX & 0xff;
  send_buf[4] = (XTS_SPCXI_DACMAX >> 8) & 0xff;
  send_buf[5] = (XTS_SPCXI_DACMAX >> 16) & 0xff;  
  send_buf[6] = (XTS_SPCXI_DACMAX >> 24) & 0xff;
  send_buf[7] = dac_max & 0xff;
  send_buf[8] = (dac_max >> 8) & 0xff;
  send_buf[9] = (dac_max >> 16) & 0xff;  
  send_buf[10] = (dac_max >> 24) & 0xff;

  //Send the command
  send_command(11);
  
  // Get ACK response from radar
  get_ack();
}


void set_frame_offset(float offset) 
{
  //Fill send buffer
  send_buf[0] = XT_START;
  send_buf[1] = XTS_SPC_X4DRIVER;
  send_buf[2] = XTS_SPCX_SET;
  send_buf[3] = XTS_SPCXI_FRAMEAREAOFFSET & 0xff;
  send_buf[4] = (XTS_SPCXI_FRAMEAREAOFFSET >> 8) & 0xff;
  send_buf[5] = (XTS_SPCXI_FRAMEAREAOFFSET >> 16) & 0xff;  
  send_buf[6] = (XTS_SPCXI_FRAMEAREAOFFSET >> 24) & 0xff;

  // Copy the bytes of the floats to send buffer
  memcpy(send_buf+7, &offset, 4);
  
  //Send the command
  send_command(11);
  
  // Get ACK response from radar
  get_ack();
}

void set_frame_area(float start_at, float stop_at)
{
  //Fill send buffer
  send_buf[0] = XT_START;
  send_buf[1] = XTS_SPC_X4DRIVER;
  send_buf[2] = XTS_SPCX_SET;
  send_buf[3] = XTS_SPCXI_FRAMEAREA & 0xff;
  send_buf[4] = (XTS_SPCXI_FRAMEAREA >> 8) & 0xff;
  send_buf[5] = (XTS_SPCXI_FRAMEAREA >> 16) & 0xff;  
  send_buf[6] = (XTS_SPCXI_FRAMEAREA >> 24) & 0xff;

  // Copy the bytes of the floats to send buffer
  memcpy(send_buf+7, &start_at, 4);
  memcpy(send_buf+11, &stop_at, 4);
  
  //Send the command
  send_command(15);
  
  // Get ACK response from radar
  get_ack();
}



void set_prf_div(uint8_t div)
{
  //Fill send buffer
  send_buf[0] = XT_START;
  send_buf[1] = XTS_SPC_X4DRIVER;
  send_buf[2] = XTS_SPCX_SET;
  send_buf[3] = XTS_SPCXI_PRFDIV & 0xff;
  send_buf[4] = (XTS_SPCXI_PRFDIV >> 8) & 0xff;
  send_buf[5] = (XTS_SPCXI_PRFDIV >> 16) & 0xff;  
  send_buf[6] = (XTS_SPCXI_PRFDIV >> 24) & 0xff;
  send_buf[7] = div;

  //Send the command
  send_command(8);
  
  // Get ACK response from radar
  get_ack();
}


// This method waits indefinitely for the XTS_SPRS_READY message from the radar
void wait_for_ready_message()
{
  SerialDebug.println("Waiting for XTS_SPRS_READY...");
  while (true) {
    if (receive_data() < 1)
      continue;

    if (recv_buf[1] != XTS_SPR_SYSTEM)
      continue;

    uint32_t response_code = (uint32_t)recv_buf[2] | ((uint32_t)recv_buf[3] << 8) | ((uint32_t)recv_buf[4] << 16) | ((uint32_t)recv_buf[5] << 24);
    if (response_code == (uint32_t)XTS_SPRS_READY) {
      SerialDebug.println("Received XTS_SPRS_READY!");
      return;
    }
    else if (response_code == (uint32_t)XTS_SPRS_BOOTING)
      SerialDebug.println("Radar is booting...");
  }
}

// This method checks if an ACK was received from the radar
void get_ack()
{
  int len = receive_data();
  
  if (len == 0)
    SerialDebug.println("No response from radar");
  else if (len < 0)
    SerialDebug.println("Error in response from radar");
  else if (recv_buf[1] != XTS_SPR_ACK)  // Check response for ACK
    SerialDebug.println("Did not receive ACK!");
}


/*
 * Adds CRC, Escaping and Stop byte to the
 * send_buf and sends it over the SerialRadar.
 */
void send_command(int len) 
{ 
  unsigned char crc = 0;
  
  // Calculate CRC
  for (int i = 0; i < len; i++)
    crc ^= send_buf[i];

  // Add CRC to send buffer
  send_buf[len] = crc;
  len++;
  
  // Go through send buffer and add escape characters where needed
  for (int i = 1; i < len; i++) {
    if (send_buf[i] == XT_START || send_buf[i] == XT_STOP || send_buf[i] == XT_ESCAPE)
    {
      // Shift following bytes one place up
      for (int u=len; u > i; u--)
        send_buf[u] = send_buf[u-1];

      // Add escape byte at old byte location
      send_buf[i] = XT_ESCAPE;

      // Increase length by one
      len++;
      
      // Increase counter so we don't process it again
      i++;
    }
  } 
  
  // Send data (including CRC) and XT_STOP
  SerialRadar.write(send_buf, len);
  SerialRadar.write(XT_STOP);

  // Print out sent data for debugging:
  SerialDebug.print("Sent: ");  
  for (int i = 0; i < len; i++) {
    SerialDebug.print(send_buf[i], HEX);  
    SerialDebug.print(" ");
  }
  SerialDebug.println(XT_STOP, HEX);
}
  
  
/* 
 * Receive data from radar module
 *  -Data is stored in the global array recv_buf[]
 *  -On success it returns number of bytes received (without escape bytes
 *  -On error it returns -1
 */
int receive_data() {

  int recv_len = 0;  //Number of bytes received

  // Wait 500 ms if nothing is available yet
  if (!SerialRadar.available())
    delay(500);
    
  // Wait for start character
  while (SerialRadar.available()) 
  {
    unsigned char c = SerialRadar.read();  // Get one byte from radar

    // If we receive an ESCAPE character, the next byte is never the real start character
    if (c == XT_ESCAPE)
    {
      // Wait for next byte and skip it.
      while (!SerialRadar.available());
      SerialDebug.print("Ignoring because escape: ");
      SerialDebug.print(SerialRadar.read(), HEX);
    }
    else if (c == XT_START) 
    {
      // If it's the start character we fill the first character of the buffer and move on
      recv_buf[0] = c;
      recv_len = 1;
      break;
    }

    // Wait 10 ms if nothing is available yet
    if (!SerialRadar.available())
      delay(10);
  }

  // Wait 10 ms if nothing is available yet
  if (!SerialRadar.available())
    delay(10);
    
  // Start receiving the rest of the bytes
  while (SerialRadar.available()) 
  {
    // read a byte
    unsigned char c = SerialRadar.read(); // Get one byte from radar

    // is it an escape byte?
    if (c == XT_ESCAPE)
    {
      // If it's an escape character next character in buffer is data and not special character:
      while (!SerialRadar.available());
      c = SerialRadar.read();
    }
    // is it the stop byte?
    else if (c == XT_STOP) {
      // Fill response buffer, and increase counter
      recv_buf[recv_len++] = c;
      break;  //Exit this loop 
    }
    
    // Fill response buffer, and increase counter
    recv_buf[recv_len++] = c;

    // Wait 10 ms if nothing is available yet
    if (!SerialRadar.available())
      delay(10);
  }

  #if 1
  // Print received data
  SerialDebug.print("Received: ");
  for (int i = 0; i < recv_len; i++) {
    SerialDebug.print(recv_buf[i], HEX);
    SerialDebug.print(" ");
  }
  SerialDebug.println(" ");
  #endif
  
  // If nothing was received, return 0.
  if (recv_len==0)
    return 0;
  
  // If stop character was not received, return with error.
  if (recv_buf[recv_len-1] != XT_STOP)
    return -1;


  //
  // Calculate CRC
  //
  unsigned char crc = 0;
  
  // CRC is calculated without the crc itself and the stop byte, hence the -2 in the counter
  for (int i = 0; i < recv_len-2; i++) 
    crc ^= recv_buf[i];
  
  // Check if calculated CRC matches the recieved
  if (crc == recv_buf[recv_len-2]) 
  {
    return recv_len;  // Return length of received data (without escape bytes) upon success
  }
  else 
  {
    SerialDebug.print("CRC mismatch: ");
    SerialDebug.print(crc, HEX);
    SerialDebug.print(" != ");
    SerialDebug.println(recv_buf[recv_len-2], HEX);
    return -1; // Return -1 upon crc failure
  } 
}



int receive_float_data_msg() {

  bool start_sequence_received = false;
  int recv_len = 0;  //Number of bytes received
  int discarded = 0; //Number of bytes discarded
  
  // Wait for XTS_FLAGSEQUENCE_START_NOESCAPE sequence (0x7C7C7C7C)
  while (!start_sequence_received) 
  {
    while (!SerialRadar.available()) {}    
      
    unsigned char c = SerialRadar.read();  // Get one byte from radar
    if (c == 0x7C)
    {
      //SerialDebug.println("First 7C received");
      while (!SerialRadar.available()) {}
      if (SerialRadar.read() != 0x7C) {
        discarded++;
        continue;
      }

      //SerialDebug.println("Second 7C received");
      while (!SerialRadar.available()) {}
      if (SerialRadar.read() != 0x7C) {
        discarded++;
        continue;
      }

      //SerialDebug.println("Third 7C received");
      while (!SerialRadar.available()) {}
      if (SerialRadar.read() == 0x7C)
        start_sequence_received = true;
      else {
        discarded++;
        continue;
      }
    }
    else {
      discarded++;
    }
  }
  
  //SerialDebug.println("Received XTS_FLAGSEQUENCE_START_NOESCAPE");

  // Receive the size of the package (in bytes) starting from after the <RESERVED> has been received
  uint32_t packet_length = 0;
  while (!SerialRadar.available()) {}
  packet_length |= SerialRadar.read();
  while (!SerialRadar.available()) {}
  packet_length |= (uint32_t)SerialRadar.read() << 8;
  while (!SerialRadar.available()) {}
  packet_length |= (uint32_t)SerialRadar.read() << 16;
  while (!SerialRadar.available()) {}
  packet_length |= (uint32_t)SerialRadar.read() << 24;

  // Read byte Reserved
  while (!SerialRadar.available()) {}
  SerialRadar.read();


  
  // Read byte XTS_SPR_DATA
  while (!SerialRadar.available()) {}
  SerialRadar.read();
  
  // Read byte XTS_SPRD_FLOAT
  while (!SerialRadar.available()) {}
  SerialRadar.read();

  // Read [ContentId(i)] + [Info(i)] + [Length(i)]
  uint32_t content_id, info, num_floats;
  while (!SerialRadar.available()) {}
  SerialRadar.readBytes((char[])&content_id, 4);
  while (!SerialRadar.available()) {}
  SerialRadar.readBytes((char[])&info, 4);
  while (!SerialRadar.available()) {}
  SerialRadar.readBytes((char[])&num_floats, 4);

  
  // Read the data items
  if (packet_length > (uint32_t)RECV_BUFFER_SIZE) {
    SerialDebug.println("Strange packet size...");
    // Seems we're out of sync. Delete old data:
    while (SerialRadar.available()) {
      SerialRadar.read();
    }
    return -1;
  }
  else {
    while (recv_len < packet_length-14) {
      while(!SerialRadar.available()){}
      recv_buf[recv_len++] = SerialRadar.read();
    }
  }

  // 
  if (SerialRadar.available())
    SerialDebug.println("Still something in buffer...");

  // It's good to know that we're discarding bytes - we're probably reading too slow (Ex: Printing out a lot of text)
  if (discarded > 0)
  {
    SerialDebug.print("# of discarded bytes: ");
    SerialDebug.println(discarded, DEC);
  }


  /*for (int i=0; i < recv_len; i++) {
    SerialDebug.print(recv_buf[i], HEX);
    SerialDebug.print(" ");
  }*/

  SerialDebug.println(" ");
  
  // Convert to floats and find peak
  int max_pos = -1;
  float max_val = 0.0;
  float * float_ptr;
  //float radar_data[1467];
  for (int i=0; i < num_floats; i++)
  {
    float_ptr = (float*)&recv_buf[i*4];
    
    if (*float_ptr > max_val) {
      max_val = *float_ptr;
      max_pos = i;
    }
    
    // There are some issues when printing floats with a lot of decimals, 
    // so here we're converting to int (and loosing resolution)
    //SerialDebug.print((int)(radar_data[i]*10000), DEC);
    
    SerialDebug.print(*float_ptr, 4);
    SerialDebug.print(", ");
  }

  SerialDebug.println("");

  /*SerialDebug.print("Packet length: ");
  SerialDebug.println(packet_length, DEC);

  SerialDebug.print("Num floats: ");
  SerialDebug.println(num_floats, DEC);
  
  SerialDebug.print("Max value: ");
  SerialDebug.print(max_val, 4);
  SerialDebug.print(" at position: ");
  SerialDebug.println(max_pos, DEC);*/


  
  // If nothing was received, return 0.
  if (recv_len==0)
    return 0;
  

}




