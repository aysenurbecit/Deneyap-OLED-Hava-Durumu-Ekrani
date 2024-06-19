#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <deneyap.h>
#include <Deneyap_OLED.h>
#include <Deneyap_DokunmatikTusTakimi.h> 

const char* ssid = "Wifi Adi";
const char* password = "Wifi Şifresi";

OLED OLED;
Keypad TusTakimi; // Keypad için class tanımlanması

String cityCodes[] = {"Ankara", "Istanbul", "Izmir", "New York", "London"}; // Şehir adlarını içeren dizi
String countryCodes[] = {"TR", "US", "GB"}; // Ülke kodlarını içeren dizi

void setup() {
  OLED.begin(0x7A); 
  OLED.clearDisplay(); 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { // WiFi baglantisini sagla
    delay(500);
    Serial.println(".");
  }
  Serial.begin(115200); // Seri haberleşme başlatılması
  if (!TusTakimi.begin(0x0E)) { // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
    Serial.println("I2C bağlantısı başarısız "); // I2C bağlantısı başarısız olursa seri terminale yazdırılması
    while (1);
  }
}

void loop() {
  int keypadDeger = TusTakimi.KeypadRead(); // Tuş takımına basılan değeri okuması
  if (keypadDeger != 0xFF) {
    Serial.println(keypadDeger); // Tuş takımından okunan değerin seri monitore yazılması
    int cityIndex = keypadDeger - 1; // Tuş takımındaki değeri şehir indeksine çevir
    if (cityIndex >= 0 && cityIndex < sizeof(cityCodes) / sizeof(cityCodes[0])) {
      String city = cityCodes[cityIndex];

      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http; // HTTPClient olustur ve OpenWeatherMap API'ye istek yap. 
        //APIKEY kısmına openweathermap sitesinden aldığınız size özel api keyi yazmalısınız.
        String url = "https://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=APIKEY";
        http.begin(url);
        int httpCode = http.GET();

        if (httpCode > 0) { // Basarili istek durumunda devam et
          String payload = http.getString();
          char json[512]; // String'i char dizisine cevir
          payload.toCharArray(json, 512);

          // JSON verisini işle
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
    delay(1000);
  }
}

