#include <Arduino.h>
#include <ETH.h>
#include <SPI.h>
#include <SD.h>
#include "config.h"

#include "ModbusClientTCP.h"
#include "ModbusCode.h"

static bool eth_connected = false;

void WiFiEvent(WiFiEvent_t event)
{
#if ESP_IDF_VERSION_MAJOR > 3
  switch (event)
  {
  case ARDUINO_EVENT_ETH_START:
    Serial.println("ETH Started");
    // set eth hostname here
    ETH.setHostname("esp32-ethernet");
    break;
  case ARDUINO_EVENT_ETH_CONNECTED:
    Serial.println("ETH Connected");
    break;
  case ARDUINO_EVENT_ETH_GOT_IP:
    Serial.print("ETH MAC: ");
    Serial.print(ETH.macAddress());
    Serial.print(", IPv4: ");
    Serial.print(ETH.localIP());
    if (ETH.fullDuplex())
    {
      Serial.print(", FULL_DUPLEX");
    }
    Serial.print(", ");
    Serial.print(ETH.linkSpeed());
    Serial.println("Mbps");
    eth_connected = true;
    break;
  case ARDUINO_EVENT_ETH_DISCONNECTED:
    Serial.println("ETH Disconnected");
    eth_connected = false;
    break;
  case ARDUINO_EVENT_ETH_STOP:
    Serial.println("ETH Stopped");
    eth_connected = false;
    break;
  default:
    break;
  }
#endif
}

void setup()
{
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);

  // SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  // if (!SD.begin(SD_CS))
  // {
  //   Serial.println("SDCard MOUNT FAIL");
  // }
  // else
  // {
  //   uint32_t cardSize = SD.cardSize() / (1024 * 1024);
  //   String str = "SDCard Size: " + String(cardSize) + "MB";
  //   Serial.println(str);
  // }

  pinMode(NRST, OUTPUT);
  digitalWrite(NRST, 0);
  delay(200);
  digitalWrite(NRST, 1);
  delay(200);
  digitalWrite(NRST, 0);
  delay(200);
  digitalWrite(NRST, 1);
  delay(200);

  ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);

  // give static ip
  IPAddress staticIp(192, 168, 0, 3);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress gateway(255, 255, 0, 0);
  ETH.config(staticIp, gateway, subnet);
}

void loop()
{
  if (eth_connected)
  {
    testClient();
  }
}