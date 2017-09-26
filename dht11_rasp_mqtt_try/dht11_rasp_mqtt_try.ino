
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTTYPE DHT11
#define DHTPIN  2


const char* ssid = "sulom";
const char* password = "12345678";
const char* mqtt_server = "10.42.0.101";
const char* clientID = "NodeMCUDevKit";
const char* outTopic = "NodeMCUout";
const char* inTopic = "NodeMCUin";

DHT dht(DHTPIN, DHTTYPE); 

float humidity, temp_c;  // Values read from sensor
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor
 
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;

  Serial.print("Message arrived on topic: [");
  Serial.print(topic);
  Serial.print("], ");
  Serial.println(message);

  if(message == "temperature, c"){
    gettemperature();
    Serial.print("Sending temperature:");
    Serial.println(temp_c);
    dtostrf(temp_c , 2, 2, msg);
    client.publish(outTopic, msg);
  } else if (message == "humidity"){
    gettemperature();
    Serial.print("Sending humidity:");
    Serial.println(humidity);
    dtostrf(humidity , 2, 2, msg);
    client.publish(outTopic, msg);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, clientID);
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();           // initialize temperature sensor

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void gettemperature() {
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
   
    previousMillis = currentMillis;   
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_c = dht.readTemperature();     // Read temperature as Celcius
    if (isnan(humidity) || isnan(temp_c)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}
