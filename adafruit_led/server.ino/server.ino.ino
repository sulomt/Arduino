#include <ESP8266WiFi.h> 

/* Set these to your desired credentials. */
const char *ssid = "sulom";
const char *password = "12345678";
WiFiServer server(80); 
WiFiClient client;


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("connecting");
    delay(500);
  }
  Serial.println("\n\rConnected!!!");
  server.begin();
  Serial.println(WiFi.localIP());
}

void loop() {
    client = server.available(); 
    
    if (client)
    {
      //Serial.println("\n[Client connected]");
      while (client.connected())
      {
        if (client.available())
        {
          Serial.print((char)client.read());
          Serial.print("Client COnnected");
          client.write("hiiiii ");
          client.flush();
        }
      }
    }
    else
    {
      Serial.println("No Client!!");
      delay(1500);
    }

}
