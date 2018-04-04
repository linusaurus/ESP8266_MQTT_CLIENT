
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>

/*
 Richard Young
 MQTT Sensor Client Control
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTTYPE DHT22  
#define DHTPIN D4
// Update these with values suitable for your network.

const char* ssid = "MonkeyRanch";
const char* password = "12345678";
const char* mqtt_server = "192.168.10.98";



WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D4,INPUT_PULLUP);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();

}

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
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP-DOOR-1","Tf8SLdVNYRArALOR1T4A",NULL)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
     
      Serial.println("Sending");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }

 
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    ++value;
StaticJsonBuffer<200> jsonBuffer;
 // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
delay(500);
JsonObject& jsonObj = jsonBuffer.createObject();

jsonObj["Temperature"] = f;
jsonObj["Celsius"] = t;
jsonObj["humidity"]=h;
char strObj[256];
jsonObj.printTo(strObj,sizeof(strObj));
Serial.println(strObj);

  
 client.publish("v1/devices/me/telemetry", strObj);   
    
  }

}
