#include <ESP8266WiFi.h>
#include <ThingsBoard.h>
#include <ArduinoJson.h>


#define WIFI_AP "High impdance Point"
#define WIFI_PASSWORD "Mostafa!1357911131517"

#define TOKEN "mostafa"
char thingsboardServer[] = "192.168.1.5";
WiFiClient wifiClient;
ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()
{
  Serial.begin(9600);
  delay(10);
  InitWiFi();
  lastSend = 0;
}

void loop()
{ 
  if ( !tb.connected() ) {
    reconnect();
    while(Serial.available()>0){ // to clear serial buffer after reconnection
      Serial.read();
    }
  }
  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    get_send_sensor_data();//
    lastSend = millis();
  }
  tb.loop();
}

double humidity;
double temperature;
double soil;
double light;

void get_send_sensor_data()
{
 if(Serial.available()>0){
    String sjson = Serial.readStringUntil('\n');
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, sjson);
    humidity = doc["Humidity"][0];
    temperature = doc["Temperature"][0];
    soil    = doc["Soil"][0];
    light   = doc["Light"][0];
    Serial.println("temp\t light");
    Serial.print(temperature);
    Serial.print("\t ");
    Serial.println(light);
  
    tb.sendTelemetryFloat("temperature", temperature);
    tb.sendTelemetryFloat("humidity", humidity);
    tb.sendTelemetryFloat("soil", soil);
    tb.sendTelemetryFloat("light", light);
  }
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("TRYING TO CONNECT to WIFI");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!tb.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("TRYING TO CONNECT to WIFI");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    if ( tb.connect(thingsboardServer, TOKEN) ) {
      Serial.println( "connected to thingsboard [DONE]" );
    } else {
      Serial.print( "not connected to thingsboard [FAILED]" );
      Serial.println( " : retrying in 5 seconds" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
