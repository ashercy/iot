#include <DFRobot_SIM7000.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Grove_LED_Bar.h>
#include <math.h>

#define serverIP        "<IP>"
#define IOT_CLIENT     "<student ID>"
#define IOT_USERNAME    ""
#define IOT_KEY         ""
#define IOT_TOPIC       "<student ID>/lab2"

#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define PIN_Light       "A0"
#define PIN_LED         3
#define DHTPIN          5
#define PIN_TX          7
#define PIN_RX          8

SoftwareSerial          mySerial(PIN_RX,PIN_TX);
DFRobot_SIM7000         sim7000(&mySerial);
// Initialize DHT sensor for normal 16mhz Arduino:
DHT dht = DHT(DHTPIN, DHTTYPE);

void readData(){
  mySerial.listen();
  while(mySerial.available()){
//    Serial.write(mySerial.read());
    Serial.print(mySerial.read());
    Serial.print(" ");
  }
}
void setup(){
  int signalStrength;
  Serial.begin(9600);
  mySerial.begin(19200);
  Serial.println("Turn ON SIM7000......");
  if(sim7000.turnON()){                                        //Turn ON SIM7000
    Serial.println("Turn ON !");
  }
  Serial.println("Set baud rate......");
  while(1){
    if(sim7000.setBaudRate(19200)){                          //Set SIM7000 baud rate from 115200 to 19200, reduce the baud rate to avoid distortion
      Serial.println("Set baud rate:19200");
      break;
    }else{
      Serial.println("Faile to set baud rate");
      delay(1000);
    }
  }

  Serial.println("Check SIM card......");
  if(sim7000.checkSIMStatus()){                                //Check SIM card
    Serial.println("SIM card READY");
  }else{
    Serial.println("SIM card ERROR, Check if you have insert SIM card and restart SIM7000");
    while(1);
  }


  Serial.println("Set net mode......");
  while(1){
    if(sim7000.setNetMode(sim7000.eNB)){                              //Set GPRS mode
      Serial.println("Set NB mode");
      break;
    }else{
      Serial.println("Fail to set mode");
      delay(1000);
    }
  }

  Serial.println("Get signal quality......");
  delay(1500);
  signalStrength=sim7000.checkSignalQuality();                 //Check signal quality from (0-30)
  Serial.print("signalStrength =");
  Serial.println(signalStrength);
  delay(500);

  Serial.println("Attaching service......");
  while(1){
    if(sim7000.attacthService()){                            //Open the connection
      Serial.println("Attach service");
      break;
    }else{
      Serial.println("Fail to Attach service");
      delay(1000);
    }
  }
  Serial.print("Connect to :");
  Serial.println(serverIP);
  if(sim7000.openNetwork(sim7000.eTCP,serverIP,1883)){                  //Connect to server
    Serial.println("Connected !");
  }else{
    Serial.println("Failed to connect");
    return;
  }
  
  delay(200);
  Serial.print("Connect to : ");
  Serial.println(IOT_USERNAME);
  if(sim7000.mqttConnect(IOT_CLIENT,IOT_USERNAME,IOT_KEY)){    //MQTT connect request
    Serial.println("Connected !");
  }else{
    Serial.println("Failed to connect");
    return;
  }
  delay(200);
  
  Serial.print("Subscribe topic: ");
  Serial.println(IOT_TOPIC);
  if(sim7000.mqttSubscribe(IOT_TOPIC)){                 //Send data to topic
    Serial.println("Send OK");
  }else{
    Serial.println("Failed to send");
    return;
  }
  delay(200);
  
  // Setup sensor:
  dht.begin();
  pinMode(PIN_LED,OUTPUT);
}

void loop(){
  char Data[50] = {'\0'};
  char payload[50] = {'\0'};
  int index = 0;
  bool flag = false;    //already get the RL or not
  bool dirty = false;   //there is packet in buffer or not
  mySerial.listen();
  while(mySerial.available()){
    dirty = true;
    int tmp = mySerial.read();
    Data[index++] = tmp;
  }
  delay(200);
  mySerial.flush();
  if(dirty){
    int RL = Data[1];
    int TPlen = Data[3];
    Serial.print("payload:");
    for(int i=TPlen+4;i<=RL+1;i++) {
      Serial.print(Data[i]);
      payload[i-(TPlen+4)] = Data[i];
    }
    Serial.println();
    delay(200);
    float lux = char2int(payload,RL-(TPlen+4),4);
//    Serial.println(lux);
    delay(200);
    if(lux<500){
      digitalWrite(PIN_LED,LOW);
      Serial.println("LOW");
      delay(500);
    }
    else{
      digitalWrite(PIN_LED,HIGH);
      Serial.println("HIGH");
      delay(500);
    }
    dirty = false;
  }
}

float char2int(char* payload,int len, int index)
{
  float out = 0.0;
  for(int i=index; i<len;i++){
    if(payload[i]=='.'){
      i++;
      out += (payload[i] - '0')*0.1;
      out += (payload[i] - '0')*0.01;
      return out;
    }
    out = out*10 + payload[i] - '0';
  }
  return out;
}
