#include <NimBLEDevice.h>
#include <NimBLEClient.h>
#define SERVER_ADDRESS "7C:9E:BD:66:56:8E"  
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define TEMPERATURE_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define HUMIDITY_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26aa"

#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h> 
const char* ssid = "MUHAMMED97";
const char* password = "1234543210";
const char* mqtt_server = "192.168.156.226";
const uint32_t mqtt_port = 8885;
const char* mqtt_user = "mohammed";
const char* mqtt_password = "1234543210";
const char* mqtt_client_id = "ESP32Client";

const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIICgzCCAimgAwIBAgIUe4uF6LwakGTqwAHxF74TeJAAl6cwCgYIKoZIzj0EAwIw
gZYxCzAJBgNVBAYTAklUMREwDwYDVQQIDAhDYWxhYnJpYTEOMAwGA1UEBwwFUmVu
ZGUxDzANBgNVBAoMBlVuaWNhbDEOMAwGA1UECwwFRGltZXMxGDAWBgNVBAMMDzE5
Mi4xNjguMTU2LjIyNjEpMCcGCSqGSIb3DQEJARYabXVoYW1tZWRhcnJtYWRhbkBn
bWFpbC5jb20wHhcNMjQwMTE2MTcwNzUyWhcNMjUwMTE1MTcwNzUyWjCBljELMAkG
A1UEBhMCSVQxETAPBgNVBAgMCENhbGFicmlhMQ4wDAYDVQQHDAVSZW5kZTEPMA0G
A1UECgwGVW5pY2FsMQ4wDAYDVQQLDAVEaW1lczEYMBYGA1UEAwwPMTkyLjE2OC4x
NTYuMjI2MSkwJwYJKoZIhvcNAQkBFhptdWhhbW1lZGFycm1hZGFuQGdtYWlsLmNv
bTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABK98gKnphdcTxcg+/Z1mf5NQPw6U
qvTYs6WPtapi5s4IXFLVU7J2qyKVzt97Nb5fVYOgU5vrhhXUNuP1qTQSt5ejUzBR
MB0GA1UdDgQWBBQxOPd5W2O7umDQfUn46iU+GQ7uBzAfBgNVHSMEGDAWgBQxOPd5
W2O7umDQfUn46iU+GQ7uBzAPBgNVHRMBAf8EBTADAQH/MAoGCCqGSM49BAMCA0gA
MEUCIQD8MxFtj5jvgzQmGlMVN9mkzEV+h2/JfDd7SjXVpo+/VAIgW50vDMrNocOD
z2vJjBkagLzHtDm1e7ni9RAxDgcQ/wI=
-----END CERTIFICATE-----
)EOF";;

const char* client_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIICKTCCAc8CFEI4FtyG/3EEc55gmM9Win69dXqEMAoGCCqGSM49BAMCMIGWMQsw
CQYDVQQGEwJJVDERMA8GA1UECAwIQ2FsYWJyaWExDjAMBgNVBAcMBVJlbmRlMQ8w
DQYDVQQKDAZVbmljYWwxDjAMBgNVBAsMBURpbWVzMRgwFgYDVQQDDA8xOTIuMTY4
LjE1Ni4yMjYxKTAnBgkqhkiG9w0BCQEWGm11aGFtbWVkYXJybWFkYW5AZ21haWwu
Y29tMB4XDTI0MDExNjE3MDc1MloXDTI2MTAxMjE3MDc1MlowgZYxCzAJBgNVBAYT
AklUMREwDwYDVQQIDAhDYWxhYnJpYTEOMAwGA1UEBwwFUmVuZGUxDzANBgNVBAoM
BlVuaWNhbDEOMAwGA1UECwwFRGltZXMxGDAWBgNVBAMMDzE5Mi4xNjguMTU2LjIy
NzEpMCcGCSqGSIb3DQEJARYabXVoYW1tZWRhcnJtYWRhbkBnbWFpbC5jb20wWTAT
BgcqhkjOPQIBBggqhkjOPQMBBwNCAATKw0FnNdBCRt3WSgf1QM/Eb49jecYV2zD+
faoiTAGEjMqZEBCGTPwDSBBujrhWqk0tfyBVpqgaett4gjTlsBPUMAoGCCqGSM49
BAMCA0gAMEUCIQDewX6YsbBfJqLp0Yk4w3kc1lHzhc8IQ6fr3t/TIYMYvgIgVwZH
69qgQRkE8fKaAjYeaWlf5MKUeT+YFtVU/IzDoa8=
-----END CERTIFICATE-----
)EOF";

const char* client_key = R"EOF(
-----BEGIN EC PRIVATE KEY-----
MHcCAQEEIAl0Tr2qM4QE57UhARlWCHKSR+9AqI8vOm6H6fciDJkfoAoGCCqGSM49
AwEHoUQDQgAEysNBZzXQQkbd1koH9UDPxG+PY3nGFdsw/n2qIkwBhIzKmRAQhkz8
A0gQbo64VqpNLX8gVaaoGnrbeII05bAT1A==
-----END EC PRIVATE KEY-----
)EOF";


NimBLEClient* nimbleClient;
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  espClient.setCACert(ca_cert);
  espClient.setCertificate(client_cert);
  espClient.setPrivateKey(client_key);
  client.setServer(mqtt_server, mqtt_port);
  NimBLEDevice::init("BLE Client");
  nimbleClient = NimBLEDevice::createClient();

  // Establish MQTT connection

}

void loop() {
  // Serial.print("Free Heap: ");
  // Serial.println(ESP.getFreeHeap());
  if (!nimbleClient->isConnected()) {
    NimBLEAddress serverAddress(SERVER_ADDRESS);
    if (nimbleClient->connect(serverAddress)) {
      delay(100);
    } else {
      delay(100);
      return;
    }
  }

  NimBLERemoteService* pRemoteService = nimbleClient->getService(SERVICE_UUID);
  if (pRemoteService == nullptr) {
    nimbleClient->disconnect();
    delay(100);
    return;
  }

  NimBLERemoteCharacteristic* pTemperatureCharacteristic = pRemoteService->getCharacteristic(TEMPERATURE_CHARACTERISTIC_UUID);
  NimBLERemoteCharacteristic* pHumidityCharacteristic = pRemoteService->getCharacteristic(HUMIDITY_CHARACTERISTIC_UUID);

  if (pTemperatureCharacteristic == nullptr || pHumidityCharacteristic == nullptr) {
    nimbleClient->disconnect();
    delay(100);
    return;
  }
    if (!client.connected()) {
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("MQTT Communication has been successfully established");

    }
  }
  float temperature = atof(pTemperatureCharacteristic->readValue().c_str());
  float humidity = atof(pHumidityCharacteristic->readValue().c_str());

  Serial.println("Received Temperature: " + String(temperature) + "°C");
  Serial.println("Received Humidity: " + String(humidity) + "%");

  // Publish temperature and humidity to MQTT topics
  char temp[10];
  char hum[10];
  sprintf(temp, "%.2f", temperature);
  sprintf(hum, "%.2f", humidity);

  client.publish("esp32/dhtReadmqttdata/temperature", temp);
  client.publish("esp32/dhtReadmqttdata/humidity", hum);


  delay(100); // Adjust the delay based on your needs
}
