#include <DFRobot_SIM7000.h>

#define serverIP        "X.X.X.X"
#define IOT_CLIENT     "<student ID>"
#define IOT_USERNAME    ""
#define IOT_KEY         ""
#define IOT_TOPIC       "<student ID>/pub"

#define PIN_TX          7
#define PIN_RX          8

SoftwareSerial          mySerial(PIN_RX,PIN_TX);
DFRobot_SIM7000         sim7000(&mySerial);

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
}

void loop(){
  char sendData[50] = {'\0'};
  Serial.println("Input data end with CRLF : ");
  delay(1000);
  readSerial(sendData);
  delay(1000);
  Serial.print("Send data :");
  Serial.print(sendData);
  Serial.println("......");
  if(sim7000.mqttPublish(IOT_TOPIC,sendData)){                 //Send data to topic
    Serial.println("Send OK");
  }else{
    Serial.println("Failed to send");
    return;
  }
  delay(200);
}
void readSerial(char result[50]){
//  Serial.println(result);
  int i = 0;
  while(1){
    while(Serial.available() > 0){
      char inChar = Serial.read();
      if(inChar == '\n'){
        result[i++] = '\0';
        while(Serial.read() >= 0);
        return;
      }
      if(i == 50){
        Serial.println("The data is too long");
        result[49] = '\0';
        while(Serial.read() >= 0);
        return;
      }
      if(inChar != '\r'){
        result[i++] = inChar;
      }
    }
  }
}
