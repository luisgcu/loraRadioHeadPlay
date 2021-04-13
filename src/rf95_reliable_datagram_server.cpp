// rf95_reliable_datagram_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging server
// with the RHReliableDatagram class, using the RH_RF95 driver to control a RF95 radio.
// It is designed to work with the other example rf95_reliable_datagram_client
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with the RFM95W 

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
#define TXPWR 13
#define F1 RH_RF95::Bw500CR45SF7
#define F2 RH_RF95::BW31_25CR48SF9
#define F3 RH_RF95::BW125CR48SF9
#define F4 RH_RF95::BW125CR45SF11_ //1715ms round trip 12bytes tx +  21 bytes back
#define F5 RH_RF95::BW41_7CR45SF8
#define F6 RH_RF95::BW62_5CR45SF10
#define F7 RH_RF95::BW62_5CR45SF11
#define F8 RH_RF95::BW250CR45SF7
#define F9 RH_RF95::BW125CR45SF7
#define F10 RH_RF95::BW62_57CR45SF7
#define F11 RH_RF95::BW125CR46SF9   //373ms round trip 12bytes tx +  21 bytes back
#define F12 RH_RF95::BW62_5CR45SF10 //1403ms round trip 12bytes tx +  21 bytes back
#define F13 RH_RF95::BW62_5CR46SF10 //1549ms round trip 12bytes tx +  21 bytes back
#define F14 RH_RF95::BW62_5CR45SF11 //2639ms round trip 12bytes tx +  21 bytes back
#define F15 RH_RF95::BW125CR45SF11  //1320ms round trip 12bytes tx +  21 bytes back
#define F16 RH_RF95::BW125CR45SF10  //120ms round trip 12bytes tx +  21 bytes back
#define F17 RH_RF95::BW31_25CR45SF8 //703msround trip 12bytes tx +  21 bytes back
#define F18 RH_RF95::BW41_7CR45SF9  //995ms round trip 12bytes tx +  21 bytes back
#define F19 RH_RF95::BW41_7CR45SF10 //1979ms round trip 12bytes tx +  21 bytes back 

// Singleton instance of the radio driver
RH_RF95 rf95(4, 2);
//RH_RF95 driver(4, 2); // Rocket Scream Mini Ultra Pro with the RFM95W

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(rf95, SERVER_ADDRESS);

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
//#define Serial SerialUSB
float frequency = 434.0;  

void setup() 
{
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
//  pinMode(4, OUTPUT);
//  digitalWrite(4, HIGH);

  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to be available
  if (manager.init())
  {
    if (!rf95.setFrequency(frequency))
      Serial.println("Unable to set RF95 frequency");
    if (!rf95.setModemConfig(F18))
      Serial.println("Invalid setModemConfig() option");
    rf95.setTxPower(TXPWR,false);
    Serial.println("RF95 radio initialized.");
  }
  else
  {
    Serial.println("RF95 radio initialization failed.");
  }
  Serial.print("RF95 max message length = ");
  Serial.println(rf95.maxMessageLength());
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 2 to 20 dBm:
//  driver.setTxPower(20, false);
  // If you are using Modtronix inAir4 or inAir9, or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for 0 to 15 dBm and with useRFO true. 
  // Failure to do that will result in extremely low transmit powers.
//  driver.setTxPower(14, true);

  // You can optionally require this module to wait until Channel Activity
  // Detection shows no activity on the channel before transmitting by setting
  // the CAD timeout to non-zero:
//  driver.setCADTimeout(10000);
Serial.println("Reliable datagram server ready");
}

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);

      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed");
    }
  }
}

