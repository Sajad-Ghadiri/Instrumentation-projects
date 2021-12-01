#define lightPin 11
#define motor_in1 7
#define motor_in2 6
#define motor_pwm 5
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
StaticJsonDocument<200> create_json();
byte bytes[4];
double ref_temp;
double ref_humidity;
double ref_soil;
double ref_light;

void temp_control(double ref_temp,double real_temp){
  int pwmVal;
  double error = ref_temp - real_temp;
  if(error >0){
    pwmVal = 0;
  }else{
    pwmVal = map(-error,0,10,0,255);
  }
  digitalWrite(motor_in1,1);
  digitalWrite(motor_in2,0);
  analogWrite(motor_pwm,pwmVal);
}

void encode_data(double d,byte* readyToSend){
  long Max = 1000000;
  long y= Max/2;
  double gain = 100;
  long number = 0;
  number = d * 100;

}
double decode_data(byte* bs){
  long Max = 1000000;
  long y= Max/2;
  double gain = 100;
  long number = 0;
  double result =0;
  for(int i=0;i<4;i++){
    number += pow(256,i) * int(bs[i]);
  }
  if(number > y){
    number = -(Max - number) ;
  }
  result = number / gain;
  return result;  
}
void setup() {
 pinMode(lightPin,OUTPUT);
 pinMode(motor_in1,OUTPUT);
 pinMode(motor_in2,OUTPUT);
 pinMode(motor_pwm,OUTPUT);
 Serial.begin(9600);
 Serial1.begin(9600);
 Serial2.begin(9600);
 Serial3.begin(9600);
 while (!Serial1) continue;
 while (!Serial2) continue;
 while (!Serial3) continue;
 while(!Serial) continue;
}

byte control_light(double ref,double real_light){
  double error = ref - real_light;
  if(error < 0){
    return 0;
  }
  byte pwm = map(error,0,10,0,255);
  return pwm;
}

double temperature_data;
double soil_data;
double light_data;
double humidity_data;
void loop() {
   //to get real data from Matlab
   receive_from_matlab();
   //jsonify and send to RPI and Server
   StaticJsonDocument<200> doc = create_json();
   serializeJson(doc,Serial2);
   Serial2.println();
   //to get references from RPI
   receive_from_rpi();
   //for control
   byte light_pwm = control_light(ref_light,light_data);
   analogWrite(lightPin,light_pwm);
   temp_control(ref_temp,temperature_data);

   //updating after a Delay
   delay(1000);
}
void receive_from_matlab(){
     if(Serial1.available()>0)
     {
      Serial1.readBytes(bytes,4);
      temperature_data = decode_data(bytes);
     }
   if(Serial3.available()>0)
     {
      Serial3.readBytes(bytes,4);
      light_data = decode_data(bytes);
     }
   if(Serial.available()>0)
     {
      Serial.readBytes(bytes,4);
      soil_data = decode_data(bytes);
     }     
}
StaticJsonDocument<200> create_json(){
   StaticJsonDocument<200> doc;
   JsonArray ja_soil = doc.createNestedArray("Soil");
   JsonArray ja_humid = doc.createNestedArray("Humidity");
   JsonArray ja_temp = doc.createNestedArray("Temperature");
   JsonArray ja_ligh = doc.createNestedArray("Light");
   ja_soil.add(soil_data);
   ja_humid.add(humidity_data);
   ja_temp.add(temperature_data);    
   ja_ligh.add(light_data);
   return doc;
}


char identifier;
int type;
double result;
void receive_from_rpi(){
  while(Serial2.available())
  {
    identifier =Serial2.read();
    if(identifier == 'S')
      type = 1;
    if(identifier == 'H')
      type = 2;
    if(identifier == 'T')
      type = 3;
    if(identifier == 'L')
      type = 4;
    if(identifier == '[')
    {
      
      result = get_value();
      if(type == 1)
        ref_soil = result;
      else if(type == 2)
        ref_humidity = result;
      else if(type == 3)
        ref_temp = result;
      else if(type == 4)
        ref_light = result;     
    }
  }
    Serial1.println(ref_light);
    Serial1.println(ref_soil);
    Serial1.println(ref_temp);
    Serial1.println(ref_humidity);
}

double get_value()
{
  int num;
  double sum = 0;
  num = Serial2.read();
  while(num > 47 && num < 58)
  {
    sum = (sum * 10) + (num - 48);
    num = Serial2.read();
  }
  return sum/100;
}
