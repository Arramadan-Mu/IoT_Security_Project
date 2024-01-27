#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <DHT.h>

#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define TEMPERATURE_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define HUMIDITY_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26aa"

BLEServer* pServer;
BLECharacteristic* pTemperatureCharacteristic;
BLECharacteristic* pHumidityCharacteristic;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("BLE Server");
  pServer = BLEDevice::createServer();

  BLEService* pService = pServer->createService(SERVICE_UUID);

  pTemperatureCharacteristic = pService->createCharacteristic(
    TEMPERATURE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ
  );

  pHumidityCharacteristic = pService->createCharacteristic(
    HUMIDITY_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ
  );

  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  dht.begin();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Update the characteristics with the latest values
  pTemperatureCharacteristic->setValue(String(temperature).c_str());
  pHumidityCharacteristic->setValue(String(humidity).c_str());

  delay(1000);
}