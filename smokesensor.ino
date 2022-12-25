/* Gerekli Kütüphaneler */
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

/* Kablosuz Bağlantı Bilgileri */
const char* WLAN_SSID = "ahmet.codes";
const char* WLAN_PASSWORD = "abc123def";

/* ThingSpeak Kurulumu */
const char* thingSpeakHost = "api.thingspeak.com";
unsigned long Channel_ID = 1989816; // Channel ID’yi buraya yazın.
const char * WriteAPIKey = "EDB5OSIV4B7Y9C3F"; // Write API Key’i buraya yazın.
const int field_no = 1; // Veriyi yazacağımız field numarası

/* WiFi Bağlantısı */
void WiFi_Setup()
{
if (WiFi.status() != WL_CONNECTED)
{
Serial.print("Kablosuz Agina Baglaniyor");
WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
// WiFi durum kontrolü
while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.print(".");
}
Serial.println();
Serial.print(WLAN_SSID);
Serial.println("Kablosuz Aga Baglandi");
}
}

//Sensör pinlerimizi tanımlıyoruz:
#define sensor_pin A0
#define buzzer_pin 5
//RGB LED'imizin çıkış pinlerini tanımlıyoruz:
#define kirmizi_led 14
#define yesil_led 12
#define mavi_led 13
int smokeLevel = 0;
WiFiClient client;

void setup()
{
  //Alarm için kullanacağımız buzzer ve LED'leri çıkış olarak tanımlıyoruz
  pinMode(buzzer_pin, OUTPUT);
  pinMode(kirmizi_led, OUTPUT);
  pinMode(yesil_led, OUTPUT);
  pinMode(mavi_led, OUTPUT);
  //Varsayılan olarak LED'in sönük kalmasını sağlıyoruz
  digitalWrite(kirmizi_led, HIGH);
  digitalWrite(yesil_led, HIGH);
  digitalWrite(mavi_led, HIGH);
  
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  Serial.begin(9600); //seri port ekranı veri alımı için haberleşme sağlanıyor.
  digitalWrite(mavi_led, LOW); // 7 saniye sensörün ısınmasını beklerken mavi ışık yanmakta
  delay(7000);
  digitalWrite(mavi_led, HIGH); //7 saniye sonra mavi ledi söndürecdek
  delay(1000);
}

void loop() 
{
  WiFi_Setup();
  delay(100);
  
  //analogRead() fonksiyonu ile sensör değerini ölçüyoruz ve bir değişken içerisinde tutuyoruz:
  int sensorDeger = analogRead(sensor_pin);
  //Sensör değeri belirlediğimiz eşik değerinden yüksek ise alarm() fonksiyonunu çağırıyoruz:
  
  smokeLevel = map(sensorDeger,0,1000,0,100);

  if (sensorDeger >= 666) //sensör değeri 300ü geçince alarm fonksiyonunu çağrıyoruz
    alarm(100);
  //Alarmın çalmadığı durumda LED'in yeşil yanmasını istiyoruz:
  else
    digitalWrite(yesil_led, LOW);
  Serial.println(sensorDeger); //seri port ekranında değerleri görmek için yazdırıyoruz.
  delay(1);
  uploadData();
}

//Alarm fonksiyonumuzu tanımlıyoruz. Bu fonksiyon parametre olarak buzzerın ötüp ve LED'in yanıp söneceği süreyi almakta.

void alarm(unsigned int duration)
{
  tone(buzzer_pin, 493); //Buzzer 493Hz de si notası çıkaracaktır. 
  //normal durumda yeşil yanmaktaydı alarm durumuna geçerken yeşil ledi söndürüp kırmızı ledi yakıyoruz.
  digitalWrite(kirmizi_led, LOW);
  digitalWrite(yesil_led, HIGH);
  delay(duration);
  noTone(buzzer_pin);
  digitalWrite(kirmizi_led, HIGH);
  digitalWrite(yesil_led, HIGH);
  delay(duration);
}
void uploadData()
{
  WiFi_Setup();

  int x;
  x = ThingSpeak.writeField(Channel_ID, field_no, smokeLevel, WriteAPIKey);
  if (x == 200)Serial.println("Veri güncellendi.");
  else
  {
    Serial.println("Veri güncelleme başarısız, tekrar deneniyor..");
    delay(20000);
    uploadData();
  }
}