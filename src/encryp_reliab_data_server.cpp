// serial_reliable_datagram_server.pde
// -*- mode: C++ -*-
// Tested with Arduino Mega, Teensy 3.1, Moteino, Arduino Due
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2 //nano atmega

RH_RF95 rf95;                               // Instanciate a LoRa driver
Speck myCipher;                             // Instantiate a Speck block ciphering
RHEncryptedDriver myDriver(rf95, myCipher); // Instantiate the driver with those two
float frequency = 434.0;                    // Change the frequency here.
unsigned char encryptkey[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
//RHReliableDatagram manager(myDriver, SERVER_ADDRESS);
RHReliableDatagram manager(rf95, SERVER_ADDRESS);

void setup()
{
  Serial.begin(115200);
  // Configure the port RH_Serial will use:
  if (!manager.init())
    Serial.println("LoRa init failed");

  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);

  // Now set up the encryption key in our cipher
  myCipher.setKey(encryptkey, sizeof(encryptkey));
  Serial.println("Setup finished");
}

uint8_t data[] = "Hello back to you";
// Dont put this on the stack:
//uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
//Sever
void loop()
{

  // Wait for a message addressed to us from the client
  //manager.waitAvailable();
  myDriver.waitAvailable();
 uint8_t buf[myDriver.maxMessageLength()];
  uint8_t len = sizeof(buf);
  uint8_t from;
  if (manager.recvfromAck(buf, &len, &from))
  {
    Serial.print("got request from : 0x");
    Serial.print(from, HEX);
    Serial.print(": ");
    Serial.println((char *)buf);

    // Send a reply back to the originator client
    if (!manager.sendtoWait(data, sizeof(data), from))
     Serial.println("sendtoWait failed");
  }
}