#include <iarduino_OLED.h>

#include <DHT.h>
#include <DHT_U.h>

#include <PubSubClient.h>
#include <ESP8266WiFi.h>


#define DHTPIN D4		// наш датчик подключен на этот контакт
#define FC A0 			// порт подключения сенсора
#define DHTTYPE DHT11   // тип датчика DHT 11

DHT dht(DHTPIN, DHTTYPE);	// инициализируем датчик

iarduino_OLED myOLED(0x3C);	//  адрес датчика
extern uint8_t MediumFontRus[]; // размер шрифта


const char* ssid = "silicon"; // WiFi name
const char* password =  "xid123mt"; //WiFi password
const char* mqttServer = "m4.wqtt.ru";
const int mqttPort = 11620;
const char* mqttUser = "u_XYU8LV";
const char* mqttPassword = "klYt4sen";
uint8_t* tempMes;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

	pinMode(FC, INPUT);
	Serial.begin(115200); 
   
	myOLED.begin(); 
	myOLED.setFont(MediumFontRus); 
	myOLED.clrScr(); 
	myOLED.print(F("Запуск"), OLED_C, 15);
	delay(1000);
	
	WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    myOLED.clrScr(); 
    myOLED.print("Connecting", 1, 15);
    myOLED.print("to WIFI...", 1, 33);

  }
    Serial.println("Connected to the WiFi network");
    myOLED.print("Successfuly", 1, 51);
    delay(1500);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    myOLED.clrScr(); // Очистим экран.
    myOLED.print("Connecting", 1, 15);
    myOLED.print("to MQTT...", 1, 33);

    delay(1000);
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
		Serial.println("connected");  
		myOLED.print("Successfuly", 1, 51);
  		delay(1000);
    //myOLED.clrScr(); // Очистим экран.
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(1000);
    }
  }
	//Serial.println("DHT11 тест!");
	dht.begin();
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}
void sendTemp(float temp){
	
 char result[4];
    dtostrf(temp, 2, 2, result);
	client.publish("/temp",result,true);
}
void sendHumi(float humi){

  char result[4];
    dtostrf(humi, 2, 2, result);
	client.publish("/humi",result,true);
}

void sendEarth(float earth){

  char result[4];
    dtostrf(earth, 2, 2, result);
	client.publish("/earth",result,true);
}

void loop() {
	delay(1000);
	float h = dht.readHumidity();
	float t = dht.readTemperature();
	float f = dht.readTemperature(true);
	float hif = dht.computeHeatIndex(f, h);
	float hic = dht.computeHeatIndex(t, h, false);
	float moisture_percentage;
	int sensor_analog;
	
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Ошибка считывания из DHT датчика!");
    return;
  }
  

  Serial.print("Влажность: ");Serial.print(h);Serial.print(" % ");
  Serial.print("Tемпература: ");  Serial.print(t);  Serial.print(" *C ");
  /*
  Serial.print(f);
  Serial.print(" *F ");
  Serial.print("Тепловой индекс: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
  */
  
	myOLED.clrScr();
	myOLED.print("Цветок", OLED_C, 15);
	myOLED.print("Темп", 1, 43);
	myOLED.print(t, 55, 43);
	myOLED.print("Влаж", 1, 61);
	myOLED.print(h, 55, 61);
	delay(1500);
  
	sendTemp(t);
	sendHumi(h);


	sensor_analog = analogRead(FC);
  sensor_analog = map(sensor_analog,1024,0,0,100);
  
	moisture_percentage = ( 100 - sensor_analog);
  Serial.println(sensor_analog);
	sendEarth(sensor_analog);

	myOLED.clrScr();
	myOLED.print("Почва", OLED_C, 15);
	myOLED.print(sensor_analog, 1, 50);


}
