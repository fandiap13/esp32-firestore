#include <Arduino.h>
#include <time.h>
#include <TemHum.h>
#include <WiFi.h>
#include <Firebase.h>

#define PEMANAS_PIN 12
#define PENDINGIN_PIN 13

unsigned long dataMillis = 0;
int count = 0;

TemHum temHumController(PEMANAS_PIN, PENDINGIN_PIN);

void setup()
{
  Serial.begin(115200);
  // Set the time zone to WIB (Waktu Indonesia Barat)
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  temHumController.initializePins();
  temHumController.wifiConnect();
  temHumController.firebaseInitialized();
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    // connect wifi
    temHumController.wifiConnect();
  }

  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  if (Firebase.ready())
  {
    // get data
    temHumController.getData();
    // PENGIRIMAN DATA SETIAP 1 MENIT
    if (dataMillis == 0 || (millis() - dataMillis) > 60000)
    {
      dataMillis = millis();
      // send data
      temHumController.sendData(timeinfo);
      count++;
    }
  }
  delay(1000);
}
