#include <DFRobot_SIM7000.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Grove_LED_Bar.h>
#include <math.h>

#define serverIP       "<IP>"
#define IOT_CLIENT     "<student ID>"
#define IOT_USERNAME    ""
#define IOT_KEY         ""
#define IOT_TOPIC       "<student ID>/lab2"

#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTPIN          5
#define PIN_TX          7
#define PIN_RX          8
#define PIN_Light       "A0"
#define PIN_LED         4

SoftwareSerial          mySerial(PIN_RX,PIN_TX);
DFRobot_SIM7000         sim7000(&mySerial);
// Initialize DHT sensor for normal 16mhz Arduino:
DHT dht = DHT(DHTPIN, DHTTYPE);
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
  // Setup sensor:
  dht.begin();
  pinMode(PIN_LED,OUTPUT);
}

void loop(){
  char sendData[50] = {'\0'};
  float light_value = analogRead(A0);
  float volts = light_value * 5.0 / 1024.0;   // 將5V電壓分成 1024等分
  float amps = volts / 10000.0;      // 電流=電壓/電阻(10K歐姆)
  float microamps = amps * 1000000;  //得到毫安培
  float lux = microamps * 2.0;       // 乘以2，得到流明
  delay(500);
  float2char(sendData,lux);
  Serial.print("Send data :");
  Serial.print(sendData);
  Serial.println("......");
  delay(200);
  if(sim7000.mqttPublish(IOT_TOPIC,sendData)){                 //Send data to topic
    Serial.println("Send OK");
  }else{
    Serial.println("Failed to send");
    return;
  }
  delay(200);
}

void float2char(char* result, float lux)
{
  int ind = 4;
  char head[4] = {'l','u','x','='};
  for(int i=0;i<4;i++){
    result[i] = head[i];
  }
  if(lux>1000){
    Serial.println("lux input wrong!");  
  }
  delay(500);
  float tmp = lux;
  for(int i=0; i<5;i++){
    delay(200);
    int n = tmp/100;
    tmp *= 10;
    tmp -= (n*1000);
    result[ind++] = (char)(n + '0');
    if(i==2) result[ind++] = '.';
  }
  return;
}
