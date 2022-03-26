# 1 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino"
# 2 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino" 2
# 3 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino" 2
# 4 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino" 2
# 5 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino" 2
# 6 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino" 2
# 7 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino" 2

# 9 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino" 2
# 10 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino" 2

#define REPORTING_PERIOD_MS 1000
#define SCREEN_WIDTH 128 /* OLED display width, in pixels*/
#define SCREEN_HEIGHT 64 /* OLED display height, in pixels*/

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(128 /* OLED display width, in pixels*/, 64 /* OLED display height, in pixels*/, &Wire, -1);

PulseOximeter pox;

uint32_t tsLastReport = 0;

BLEServer *pServer = 
# 22 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino" 3 4
                    __null
# 22 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino"
                        ;
BLECharacteristic *pCharacteristic = 
# 23 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino" 3 4
                                    __null
# 23 "c:\\Users\\Hevar\\Documents\\ardpro\\main.ino"
                                        ;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    BLEDevice::startAdvertising();
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

void onBeatDetected()
{
  Serial.println("Beat!");
}

void setup()
{
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("HRPO Sensor");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService("4fafc201-1fb5-459e-8fcc-c5c9c331914b");

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
      "beb5483e-36e1-4688-b7f5-ea07361b26a8",
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);

  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");

  Serial.print("Initializing pulse oximeter..");

  if (!display.begin(0x02 /*|< Gen. display voltage from 3.3V*/, 0x3C))
  {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(1 /*|< Draw 'on' pixels*/ /*|< Draw 'on' pixels*/);
  display.setCursor(0, 20);
  display.println("Initializing sensor...");
  display.display();
  delay(2000);

  // if (!pox.begin())
  // {
  //     Serial.println("FAILED");
  //     for (;;)
  //         ;
  // }
  // else
  // {
  //     Serial.println("SUCCESS");
  // }

  // pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
  int hr = random(50, 130);
  int spo2 = random(90, 100);

  Serial.println();

  Serial.print("Heart rate: ");
  Serial.print(hr);
  Serial.print("bpm / SpO2: ");
  Serial.print(spo2);
  Serial.println("%");

  // tsLastReport = millis();

  String str = "";
  str += hr;
  str += ",";
  str += spo2;
  pCharacteristic->setValue((char *)str.c_str());
  pCharacteristic->notify();

  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Heart rate: ");
  display.print(hr + 'bpm');
  display.println();
  display.print("SpO2: ");
  display.print(spo2);
  display.println("%");
  display.display();
  delay(1000);

  // pox.update();

  // // notify changed value
  // if (deviceConnected)
  // {
  //     if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  //     {
  //         Serial.print("Heart rate: ");
  //         Serial.print(pox.getHeartRate());
  //         Serial.print("bpm / SpO2: ");
  //         Serial.print(pox.getSpO2());
  //         Serial.println("%");

  //         tsLastReport = millis();

  //         String str = "";
  //         str += pox.getHeartRate();
  //         str += ",";
  //         str += pox.getSpO2();
  //         pCharacteristic->setValue((char *)str.c_str());
  //         pCharacteristic->notify();
  //     }

  //     // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
  // }
  // // disconnecting Serial.println("start advertising");
  // oldDeviceConnected = deviceConnected;
  // if (!deviceConnected && oldDeviceConnected)
  // {
  //     delay(500);                  // give the bluetooth stack the chance to get things ready
  //     pServer->startAdvertising(); // restart advertising
  // }
  // // connecting
  // if (deviceConnected && !oldDeviceConnected)
  // {
  //     // do stuff here on connecting
  //     oldDeviceConnected = deviceConnected;
  // }
}
