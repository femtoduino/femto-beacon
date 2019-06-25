/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <sstream>
#include <iostream>
#include <string>
#include <vector>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
uint16_t mtu = 185;
std::string testString = "0123456789ABCDEFGHIJklmnopqrstUVWXYZ.,!*This is a string here now";

// FemtoBeacon ESP32 coin (ESP32-D2WD)
// Note about RGB LED pins: HIGH turns LED completely off, LOW turns it all the way on. Use PWM on pinB (Blue).
//
// ESP32 PWM LED code
// See https://randomnerdtutorials.com/esp32-pwm-arduino-ide/
const int pinR = 26; // GPIO26
const int pinG = 4;  // GPIO4, Channel 0
const int pinB = 5;  // GPIO5
const int pwmFrequencyLED = 5000;
const int pwmDutyCycleResolution = 8;
const int ledChannelR = 0;
const int ledChannelG = 1;
const int ledChannelB = 2;
int ledDutyCycleR = 0x00;
int ledDutyCycleG = 0x00;
int ledDutyCycleB = 0x00;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// Command settings
const int COMMAND_DELIMITER_MAX = 10;

std::vector<std::string> tokenizeInput(std::string value, char delimeter) {
  std::vector<std::string> command;
  std::istringstream commandStream(value);
  std::string buff;

  // We set a limit to token character count! We don't want a bazillion tokens to parse! Safety first.
  byte delimiterCount = 0;
  while ( std::getline(commandStream, buff, delimeter ) ) {
    std::cout << buff << std::endl;
    command.push_back(buff);
    ++delimiterCount;

    if (delimiterCount >= COMMAND_DELIMITER_MAX)
      break;
  }

  return command;
}

void processCommand(std::string value) {
  std::vector<std::string> command = tokenizeInput(value, ':');
  std::vector<std::string> params;

  int commandSize = command.size();
  int paramSize = 0;

  if (commandSize > 0) {

    // SET_RGB:0xff,0xff,0xff
    // Where 0xFF is the color value (R, G, B)
    if (command[0] == "SET_RGB") {
      // Set according to 3 comma separated values
      if (commandSize == 2) {
        Serial.println("info:SET_RGB");
        params = tokenizeInput(command[1], ',');
        paramSize = params.size();

        if (paramSize == 3) {
          // Ok, parse to int. (https://en.cppreference.com/w/cpp/string/basic_string/stol)
          ledDutyCycleR = hexToDec(params[0]);
          ledDutyCycleG = hexToDec(params[1]);
          ledDutyCycleB = hexToDec(params[2]);

          if (ledDutyCycleR < 0) ledDutyCycleR = 0;
          if (ledDutyCycleG < 0) ledDutyCycleG = 0;
          if (ledDutyCycleB < 0) ledDutyCycleB = 0;

          if (ledDutyCycleR > 0xff) ledDutyCycleR = 0xff;
          if (ledDutyCycleG > 0xff) ledDutyCycleG = 0xff;
          if (ledDutyCycleB > 0xff) ledDutyCycleB = 0xff;

          runRGB();
        } else {
          Serial.println("err:INVALID_PARAM_COUNT");
        }
      } else {
        // Invalid amount of parameters
        Serial.println("err:MISSING_PARAMS");
      }
    } else if (command[0] == "TEST_RGB") {

      testRGBLED();
    } else {
      Serial.println("err:COMMAND_NOT_FOUND");
    }

  }
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");

        ::processCommand(rxValue);
      }
    }
};

void setup() {
  
  // Create the BLE Device
  BLEDevice::init("UART Service");
  BLEDevice::setMTU(mtu);
  delay(100);
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE
                                          );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();


  Serial.begin(9600); // Serial needs to be at 9600, as 115200 triggers brown out reset without extra voltage on the 3.3V pin
  

  // Setup our RGB LED pins
  setupRGBLED();
  Serial.println("BLE UART Ready");
}

void testRGBLED() {
  Serial.println("TESTING RGB");

  ledDutyCycleR = 0;
  ledDutyCycleG = 0;
  ledDutyCycleB = 0;

  runRGB();

  delay(500);

  ledDutyCycleR = 0xff;
  ledDutyCycleG = 0;
  ledDutyCycleB = 0;

  runRGB();

  delay(500);

  ledDutyCycleR = 0;
  ledDutyCycleG = 0xff;
  ledDutyCycleB = 0;

  runRGB();

  delay(500);

  ledDutyCycleR = 0;
  ledDutyCycleG = 0;
  ledDutyCycleB = 0xff;

  runRGB();

  delay(500);

  ledDutyCycleR = 0;
  ledDutyCycleG = 0;
  ledDutyCycleB = 0;

  runRGB();
}

void setupRGBLED() {
  ledcSetup(ledChannelR, pwmFrequencyLED, pwmDutyCycleResolution);
  ledcAttachPin(pinR, ledChannelR);

  ledcSetup(ledChannelG, pwmFrequencyLED, pwmDutyCycleResolution);
  ledcAttachPin(pinG, ledChannelG);

  ledcSetup(ledChannelB, pwmFrequencyLED, pwmDutyCycleResolution);
  ledcAttachPin(pinB, ledChannelB);

  runRGB();
  Serial.println("RGB pins setup");
}

void runRGB() {
  ledcWrite(ledChannelR, 256 - ledDutyCycleR);
  ledcWrite(ledChannelG, 256 - ledDutyCycleG);
  ledcWrite(ledChannelB, 256 - ledDutyCycleB);
}

void loop() {

  //runRGB();


  if (deviceConnected) {
    // pTxCharacteristic->setValue(&txValue, 1);
    pTxCharacteristic->setValue(testString);
    pTxCharacteristic->notify();
    txValue++;
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}

// See https://github.com/benrugg/Arduino-Hex-Decimal-Conversion/blob/master/hex_dec.ino
unsigned int hexToDec(std::string hexString) {

  unsigned int decValue = 0;
  int nextInt;
  char chr;
  char* validChars = "0123456789abcdefABCDEF";
  byte validChars_length = strlen(validChars);
  bool is_valid_char;

  for (int i = 0; i < hexString.size(); i++) {
    chr = hexString.at(i);
    is_valid_char = false;
    for (int j = 0; j < validChars_length; j++) {
      is_valid_char = chr == validChars[j];

      if (is_valid_char) break;
    }

    if (is_valid_char) {

      nextInt = int(hexString.at(i));

      if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
      if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
      if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
      nextInt = constrain(nextInt, 0, 15);

      decValue = (decValue * 16) + nextInt;
    }
  }

  return decValue;
}
