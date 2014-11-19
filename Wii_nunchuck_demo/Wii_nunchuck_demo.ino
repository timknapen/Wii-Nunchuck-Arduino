  /*********************
  Wii Nunchuck Demo
  
  
  Wires to pins:
                    UNO  DUE
  white   ground
  red     3.3+v
  green   data      A4    SDA (20)
  yellow  clock     A5    SCL (21)
  
  The nunchuck is only supposed to get 3.3+ volts. (So far it has worked fine at 5 volts, but be warned.)
  
  Since the nunchuck uses "Fast" I2C, we will need to change the default speed:
  #define TWI_FREQ 400000L.
  
  For the Arduino to communicate with the nunchuck, it must send a handshake.
  First send 2 bytes "0x40,0x00".
  Send one byte "0x00" each time you request data from the nunchuck.
  The data from the nunchuck will come back in 6 byte chunks.
  
  Based upon original code from windmeadow.com
  
  *********************/
  
  #define TWI_FREQ 400000L
  
  #include <Wire.h>
  
  // The data from the nunchuck will come back in 6 byte chunks.
  #define BUFFERSIZE 6
  uint8_t outbuf[BUFFERSIZE];
  int bufPos = 0;
  
  long refreshTime = 1000 / 30; // how long between updates? in millisecs
  long lastTime = 0;
  
  //______________________________________________________
  void setup()
  {
    Serial.begin (57600);
    Wire.begin ();
    nunchuck_init ();
    Serial.println("\r\n");
    Serial.print ("Start Wii Nunchuk demo\n");
    Serial.print ("joy_x\tjoy_y\tacc_x\tacc_y\tacc_z\tz_but\tc_but\r\n");
  }
  
  //______________________________________________________
  void loop()
  {
    long now = millis(); // get the current time
  
    // if we have waited long enough since the last request..
    if ( now - lastTime >= refreshTime) {
      lastTime = now; //save the current time
  
      Wire.requestFrom (0x52, BUFFERSIZE); // request data from the nunchuck
  
      // copy the received data in the buffer
      while (Wire.available () && bufPos < BUFFERSIZE) {
        outbuf[bufPos] = nunchuk_decode_byte (Wire.read());
        bufPos++;
      }
  
      if (bufPos == BUFFERSIZE) { // we've received a full buffer
        printNunchuckData();
      }
      
      // reset the buffer position
      bufPos = 0;
      send_zero();
    }
  
  }
  
  
  //______________________________________________________
  void nunchuck_init()
  {
    Wire.beginTransmission (0x52);
    Wire.write (0x40);
    Wire.write (0x00);
    Wire.endTransmission ();
  }
  
  //______________________________________________________
  void send_zero()
  {
    Wire.beginTransmission (0x52);
    Wire.write (0x00);
    Wire.endTransmission ();
  }
  
  //______________________________________________________
  void printNunchuckData()
  {
  
    int joy_x_axis = outbuf[0];
    int joy_y_axis = outbuf[1];
    int accel_x_axis = outbuf[2] * 2 * 2;
    int accel_y_axis = outbuf[3] * 2 * 2;
    int accel_z_axis = outbuf[4] * 2 * 2;
    int z_button = 0;
    int c_button = 0;
    // byte outbuf[5] contains bits for z and c buttons
    // it also contains the least significant bits for the accelerometer
    // so we have to check each bit of byte outbuf[5]
    if ((outbuf[5] >> 0) & 1) {
      z_button = 1;
    }
    if ((outbuf[5] >> 1) & 1) {
      c_button = 1;
    }
    if ((outbuf[5] >> 2) & 1) {
      accel_x_axis += 2;
    }
    if ((outbuf[5] >> 3) & 1) {
      accel_x_axis += 1;
    }
    if ((outbuf[5] >> 4) & 1) {
      accel_y_axis += 2;
    }
    if ((outbuf[5] >> 5) & 1) {
      accel_y_axis += 1;
    }
    if ((outbuf[5] >> 6) & 1) {
      accel_z_axis += 2;
    }
    if ((outbuf[5] >> 7) & 1) {
      accel_z_axis += 1;
    }
  
    // Joystick data
    Serial.print (joy_x_axis, DEC);
    Serial.print ("\t");
  
    Serial.print (joy_y_axis, DEC);
    Serial.print ("\t");
  
  // Accelerometer data
    Serial.print (accel_x_axis, DEC);
    Serial.print ("\t");
  
    Serial.print (accel_y_axis, DEC);
    Serial.print ("\t");
  
    Serial.print (accel_z_axis, DEC);
    Serial.print ("\t");
  
  // Buttons
    Serial.print (z_button, DEC);
    Serial.print ("\t");
  
    Serial.print (c_button, DEC);
    Serial.print ("\r\n");
  }
  
  //______________________________________________________
  char nunchuk_decode_byte (char x)
  {
    x = (x ^ 0x17) + 0x17;
    return x;
  }

