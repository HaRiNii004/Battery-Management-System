
#include <WiFi.h>
#include <PubSubClient.h>
float Rsh = 0.0015;
int R1 = 100;
int R2 = 10;
#define Voltage_topic "Voltage"
#define Current_topic "Current"
#define Temperature_topic "Temperature"
#define CURRENTPIN 35
#define VOLTAGEPIN 32
#define THERMISTORPIN 33

const char* ssid = "H";
const char* password = "pass";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecing to ");
  Serial.println(ssid);

  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" ");
  Serial.println("WiFi Connected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup(){
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  client.publish("Voltage", "");
  delay(1000);

  int aDC = analogRead(VOLTAGEPIN);
  float VOUT = (aDC * 3.3)/4096;
  float VIN = VOUT * (R1+R2)/R2;
  Serial.print("ADC: ");
  Serial.println(String(aDC).c_str());
  client.publish(Voltage_topic,String(aDC).c_str(),true);
  Serial.print("Vout: ");
  Serial.println(String(VOUT).c_str());
  client.publish(Voltage_topic,String(VOUT).c_str(),true);
  
  Serial.print("Vin: ");
  Serial.println(String(VIN).c_str());
  client.publish(Voltage_topic,String(VIN).c_str(),true);

 
  int adc = analogRead(CURRENTPIN);
  float Vout = (adc * 3.3)/4096;
  float Vsh = Vout/69;
  float Ish = Vsh/Rsh;
   Serial.print("ADC: ");
  Serial.println(String(aDC).c_str());
  client.publish(Current_topic,String(aDC).c_str(),true);
  Serial.print("Vsh: ");
  Serial.println(String(Vsh).c_str());
  client.publish(Current_topic,String(Vsh).c_str(),true);
  Serial.print("Ish ");
  Serial.println(String(Ish).c_str());
  client.publish(Current_topic,String(Ish).c_str(),true);
                                  
  int thermistor_adc_val;
  double output_voltage, Rth, Rth_ln, temperature;
  thermistor_adc_val = analogRead(THERMISTORPIN);
  output_voltage = ( (thermistor_adc_val * 3.3) / 4096.0 );
  Rth = ( ( 5 * ( 10.0 / output_voltage ) ) - 10 );
  Rth = Rth * 1000 ;
  Rth_ln = log(Rth);
  temperature = ( 1 / ( 0.001129148 + ( 0.000234125 * Rth_ln ) + ( 0.0000000876741 * Rth_ln * Rth_ln * Rth_ln ) ) );
  temperature = temperature - 273.15;
  Serial.print("Temperature in degree Celsius =  ");
  Serial.println(String(temperature).c_str());
  client.publish(Temperature_topic,String(temperature).c_str(),true);
  
  Serial.print("\t\t");
  Serial.println(String("\t\t").c_str());
  client.publish(Current_topic,String("\t\t").c_str(),true);
  Serial.print("Resistance in ohms = ");
  Serial.println(String("Resistance in ohms = ").c_str());
  client.publish(Voltage_topic,String("Resistance in ohms = ").c_str(),true);
  Serial.print("\n\n");
 
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT Connection...");
    if(client.connect("2e42c192-9469-4d1c-926b-e18240a93eb9")){
      Serial.println("Connected");
      client.subscribe("Volage");
      Serial.println("TOPIC SUB");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print("try again in 5 seconds");
      delay(1000);
    }
  }
}
