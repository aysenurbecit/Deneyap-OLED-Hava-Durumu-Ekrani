#include <HTTPClient.h>
#include <WiFi.h>
//#include <HttpClient.h>
#include <ArduinoJson.h>
#include <deneyap.h>
#include <Deneyap_OLED.h>

const char* ssid = "WiFi_Adi";
const char* password = "WiFi_Sifresi";

OLED OLED;

void setup() {
  OLED.begin(0x7A); 
  OLED.clearDisplay(); 
  Serial.begin(115200); // Seri haberlesmeyi baslat
 
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) { // WiFi baglantisini sagla
    delay(500);
    Serial.println(".");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http; // HTTPClient olustur ve OpenWeatherMap API'ye istek yap
    http.begin("https://api.openweathermap.org/data/2.5/weather?q=CITY_NAME,LANGUAGE_CODE&APPID=YOUR_API_KEY");
    int httpCode = http.GET();

    if (httpCode > 0) { // Basarili istek durumunda devam et
      String payload = http.getString();
      
      char json[512]; // String'i char dizisine cevir
      payload.toCharArray(json, 512);

      // JSON verisini isle
      StaticJsonDocument<512> doc;
      deserializeJson(doc, json);

      String location = doc["name"];
      String status = doc["weather"][0]["main"];
      float temp = doc["main"]["temp"];
      float humidity = doc["main"]["humidity"];
      int pressure = doc["main"]["pressure"];
      
      float tempC = temp - 273.15;

      OLED.clearDisplay();
      OLED.setTextXY(0, 0);
      OLED.putString(location + "-");
      OLED.putString(status);
      OLED.setTextXY(2, 0);
      OLED.putString("Sicaklik: " + String(tempC) + "C");
      OLED.setTextXY(4, 0);
      OLED.putString("Nem: " + String(humidity) + "%");
      OLED.setTextXY(6, 0);
      OLED.putString("Basinc: " + String(pressure) + "hPa");
  
      delay(10000); // 10 saniye bekle
    }

    http.end(); // HTTP istegini sonlandir
  }
}