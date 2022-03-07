// Include the libraries:
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Grove_LED_Bar.h>
#include <math.h>
// Set DHT pin:
#define FanPIN    2
#define LEDPIN    3
#define MotorPIN  4
#define DHTPIN    5
#define soidPIN   9
//pin 6 for LED Bar, pin 7 and pin 8 for RX and TX.
// Set DHT type, uncomment whatever type you're using!
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Initialize DHT sensor for normal 16mhz Arduino:
DHT dht = DHT(DHTPIN, DHTTYPE);
Grove_LED_Bar bar(7, 6, 0, LED_BAR_10);
void setup() {
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
  // Setup sensor:
  dht.begin();
  bar.begin();
  pinMode(soidPIN,INPUT);
  pinMode(FanPIN,OUTPUT);
  pinMode(FanPIN,OUTPUT);
  pinMode(MotorPIN,OUTPUT);
  pinMode(LEDPIN,OUTPUT);
}

void loop() {
  // Wait a few seconds between measurements:
  delay(1000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  // Read the humidity in %:
  float h = dht.readHumidity();
  // Read the temperature as Celsius:
  float t = dht.readTemperature();
  // Read the temperature as Fahrenheit:
  float f = dht.readTemperature(true);
  // read soid
  int soil = analogRead(soidPIN); 
  //read light
  float light_value = analogRead(A0);
  float volts = light_value * 5.0 / 1024.0;   // 將5V電壓分成 1024等分
  float amps = volts / 10000.0;      // 電流=電壓/電阻(10K歐姆)
  float microamps = amps * 1000000;  //得到毫安培
  float lux = microamps * 2.0;       // 乘以2，得到流明
  
  // Compute heat index in Fahrenheit (default):
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius:
  float hic = dht.computeHeatIndex(t, h, false);
  

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" % ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" \xC2\xB0");
  Serial.print("C | ");
  Serial.print(f);
  Serial.print(" \xC2\xB0");
  Serial.print("F ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" \xC2\xB0");
  Serial.print("C | ");
  Serial.print(hif);
  Serial.print(" \xC2\xB0");
  Serial.print("F");
  Serial.print(" \xC2\xB0");
  Serial.print("Soil: ");
  Serial.print(soil);
  Serial.print(" \xC2\xB0");
  //read light
  light_value = analogRead(A0);
  volts = light_value * 5.0 / 1024.0;   // 將5V電壓分成 1024等分
  amps = volts / 10000.0;      // 電流=電壓/電阻(10K歐姆)
  microamps = amps * 1000000;  //得到毫安培
  lux = microamps * 2.0;       // 乘以2，得到流明
  Serial.print ("  Light lux: ");
  Serial.println (lux);
  
  for(int i=0; i<=10; i++){
    bar.setLevel(i);
    delay(100);
  }
  digitalWrite(FanPIN,LOW);
  digitalWrite(MotorPIN,LOW);
  digitalWrite(LEDPIN,LOW);
  delay(500);
  
  for(int i=10; i>=0; i--){
    bar.setLevel(i);
    delay(100);
  }
  digitalWrite(FanPIN,HIGH);
  digitalWrite(MotorPIN,HIGH);
  digitalWrite(LEDPIN,HIGH);
  delay(3000);
}
