#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define wifi_ssid "sulom"
#define wifi_password "1234567890"

#define mqtt_server "192.168.43.200" //ip of pi

#define humidity_topic "Humidity"
#define temperature_topic "Temperature"

#define DHTTYPE DHT11
#define DHTPIN  2

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

	void setup(){
  		Serial.begin(115200);
  		dht.begin();
  		setup_wifi();
  		client.setServer(mqtt_server, 1883);
	}

	void setup_wifi() {
  		delay(10);
  		Serial.println();
  		Serial.print("Connecting to ");
  		Serial.println(wifi_ssid);

  		WiFi.begin(wifi_ssid, wifi_password);

 		 while (WiFi.status() != WL_CONNECTED) {
    			delay(500);
   			 Serial.print(".");
  		}

  		Serial.println("");
  		Serial.println("WiFi connected");
  		Serial.println("IP address: ");
  		Serial.println(WiFi.localIP());
	}

	void reconnect() {
 
  		while (!client.connected()) {
    		Serial.print("Attempting MQTT connection...");
	    	if (client.connect("ESP8266Client")) {
      		Serial.println("connected");
   		 } 
		else {
     		 Serial.print("failed, rc=");
     		 Serial.print(client.state());
      		 Serial.println(" try again in 5 seconds");		
      		 delay(5000);
   		 }
  		}
	}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();

   // if (checkBound(newTemp, temp, diff)) {
      //temp = newTemp;
      Serial.print("New temperature:");
      Serial.println(String(newTemp).c_str());
      client.publish(temperature_topic, String(newTemp).c_str(), true);
   // }

   // if (checkBound(newHum, hum, diff)) {
     // hum = newHum;
      Serial.print("New humidity:");
      Serial.println(String(newHum).c_str());
      client.publish(humidity_topic, String(newHum).c_str(), true);
    //}
  }
}
