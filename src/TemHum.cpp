#include <TemHum.h>
#include <Firebase_ESP_Client.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// DEFINE FIREBASE
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

#define WIFI_SSID "P"
#define WIFI_PASSWORD "fandiaziz"

#define API_KEY "AIzaSyDcOR7jccgDQa4UpBom546w6gqHCJv-oNg"
#define FIREBASE_PROJECT_ID "esp32-firestore-40b81"
#define USER_EMAIL "andiazizp123@gmail.com"
#define USER_PASSWORD "@Fandiaz123"

TemHum::TemHum(int pin1, int pin2)
{
    pinPemanas = pin1;
    pinPendingin = pin2;
}

void TemHum::initializePins() {
      pinMode(LED_BUILTIN, OUTPUT);
    pinMode(pinPemanas, OUTPUT);
    pinMode(pinPendingin, OUTPUT);
    digitalWrite(pinPemanas, LOW);
    digitalWrite(pinPendingin, LOW);
}

void TemHum::firebaseInitialized()
{
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    config.api_key = API_KEY;

    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
    // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
    fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

    // Limit the size of response payload to be collected in FirebaseData
    fbdo.setResponseSize(2048);

    Firebase.reconnectNetwork(true);
    Firebase.begin(&config, &auth);
}

void TemHum::wifiConnect()
{
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Connecting");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.print("Wifi Connected to ");
    Serial.println(WiFi.SSID());
    digitalWrite(LED_BUILTIN, HIGH);
}

void TemHum::getData()
{
    DynamicJsonDocument doc(1024);
    String documentPath = "control/fH0d1r91O2Q8fnix2Oa6";
    String mask = "";
    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), mask.c_str()))
    {
        String result = fbdo.payload().c_str();
        deserializeJson(doc, result);

        String pendingin = doc["fields"]["pendingin"]["stringValue"].as<const char *>();
        String pemanas = doc["fields"]["pemanas"]["stringValue"].as<const char *>();

        digitalWrite(pinPendingin, String(pendingin) == "ON" ? HIGH : LOW);
        digitalWrite(pinPemanas, String(pemanas) == "ON" ? HIGH : LOW);

        Serial.println("================================================");
        // Serial.println("Pin pendingin: " + String(pinPendingin));
        // Serial.println("Pin pemanas: " + String(pinPemanas));
        // Serial.print("Pendingin status: ");
        // Serial.println(pendingin == "ON" ? HIGH : LOW);
        // Serial.print("Pemanas status: ");
        // Serial.println(pemanas == "ON" ? HIGH : LOW);
        // Serial.println("");
        Serial.println("Pendingin : " + String(pendingin));
        Serial.println("pemanas : " + String(pemanas));
    }
    else
    {
        Serial.println(fbdo.errorReason());
    }
}

void TemHum::sendData(struct tm timeinfo)
{
    FirebaseJson content;
    // Format the date and time
    char formattedDate[21];
    strftime(formattedDate, sizeof(formattedDate), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
    // String date = "2014-10-02T15:01:23Z";
    // Serial.println(formattedDate);
    String documentPath = "tempHum";
    // save dummy data temperature
    content.set("fields/temperature/doubleValue", random(1, 500) / 100.0);
    content.set("fields/humidity/doubleValue", random(1, 500) / 100.0);
    content.set("fields/date/timestampValue", formattedDate);

    Serial.println("Create a document... ");

    if (!Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw()))
    {
        Serial.println(fbdo.errorReason());
    } else {
        Serial.println("Create document success !");
    }
}