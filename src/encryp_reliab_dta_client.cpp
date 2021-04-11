// serial_reliable_datagram_client.pde
// -*- mode: C++ -*-
// ************************************************ CLIENT SENDER Moteino ******************************************
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

RH_RF95 rf95(4, 2); // Instanciate a LoRa driver (4,2)Moteino Mega
Speck myCipher;     // Instantiate a Speck block ciphering
RHEncryptedDriver myDriver(rf95, myCipher);
float frequency = 434.0; // Change the frequency here.
unsigned char encryptkey[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
RHReliableDatagram manager(rf95, CLIENT_ADDRESS);
//RHReliableDatagram manager(myDriver, CLIENT_ADDRESS);

void setup()
{
  Serial.begin(115200);
  // Configure the port RH_Serial will use:
  Serial.println("Starting client.. ");
  if (!manager.init())
    Serial.println("LoRa init failed");

  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);

  // Now set up the encryption key in our cipher
  myCipher.setKey(encryptkey, sizeof(encryptkey));
}

uint8_t data[] = "Hello World!";
// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  Serial.println("Client Sending from MOTEINO MEGA TO Arduino Nano Aliexpress");

  // Send a message to manager_server
  if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAckTimeout(buf, &len, 3000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char *)buf);
    }
    else
    {
      Serial.println("No reply, is serial_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
  delay(5000);
}