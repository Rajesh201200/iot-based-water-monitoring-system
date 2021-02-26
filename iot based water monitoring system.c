#include<LiquidCrystal.h>
LiquidCrystal lcd(7,6,5,4,3,2);
#include<Timer.h>
Timer t;
#include <SoftwareSerial.h>
SoftwareSerial Serial1(8,9);

const int Echo1Pin    = 10;
const int Trigger1Pin = 11;

volatile float maxDistance1 = 10.00;
volatile float Duration1, DistanceCm1;

#define spr  12
const int mosPin  = A0;  // Analog input pin that the potentiometer is attached to
int mosValue = 0;        // value read from the pot
int cnt = 0;


char *api_key="NN23EO93L0SWIS5L";     // Enter your Write API key from ThingSpeak
static char postUrl[150];

void httpGet(String ip, String path, int port=80);

void setup()
{
  pinMode(spr, OUTPUT);
  pinMode(Trigger1Pin, OUTPUT);    // ultrasonic1
 pinMode(Echo1Pin, INPUT);
 lcd.begin(16, 2);
 lcd.clear();
 lcd.print("WATER MONITORING");
 lcd.setCursor(0,1);
 lcd.print(" SYSTEM - IOT ");
 delay(2000);

 Serial1.begin(115200);
 Serial.begin(9600);
 digitalWrite(spr,HIGH);
 lcd.clear();
 lcd.print("WIFI Connecting");
 Serial.println("WIFI Connecting");
 lcd.setCursor(0,1);
 lcd.print("Please wait....");
 Serial.println("Please wait....");
 Serial.println("Connecting Wifi....");
 connect_wifi("AT",1000);
 connect_wifi("AT+CWMODE=1",1000);
 connect_wifi("AT+CWQAP",1000);  
 connect_wifi("AT+RST",5000);
 connect_wifi("AT+CWJAP=\"CED-GH\",\"Ced@7619\"",10000);
 Serial.println("Wifi Connected"); 
 lcd.clear();
 lcd.print("WIFI Connected.");
  t.every(40000, send2server);
}

void loop()
{
  checkDistance1();          //  distance check
  lcd.setCursor(0,1);
  lcd.print("Mois:");
  mosValue = analogRead(mosPin);
  lcd.print(mosValue);   // Printing temperature on LCD
  lcd.print(" %    ");
  delay(2000);
  if((mosValue>800)&&(DistanceCm1<maxDistance1))
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Water Level Full");
    lcd.setCursor(0,1);
    lcd.print("Mois LOW,Pump On");
    digitalWrite(spr,LOW);
      delay(1000);
    }
   if(mosValue<400)
  {
     lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mois Level High");
    lcd.setCursor(0,1);
    lcd.print("Water Pump Off");
    digitalWrite(spr,HIGH);
      delay(1000);
    }
     if(DistanceCm1>60)
  {
     lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Water Level LOW");
    lcd.setCursor(0,1);
    lcd.print("Water Pump Off");
    digitalWrite(spr,HIGH);
      delay(1000);
    }
  t.update();
}

void send2server()
{
   lcd.clear();
  lcd.print("DATA Updating.."); 
  delay(1000);
  char distStr[8];
  char mosStr[8];
  dtostrf(DistanceCm1, 5, 3, distStr);
  dtostrf(mosValue, 5, 3, mosStr);
  sprintf(postUrl, "update?api_key=%s&field1=%s&field2=%s",api_key,distStr,mosStr);
  httpGet("api.thingspeak.com", postUrl, 80);
}

//GET https://api.thingspeak.com/update?api_key=SIWOYBX26OXQ1WMS&field1=0

void httpGet(String ip, String path, int port)
{
  int resp;
  String atHttpGetCmd = "GET /"+path+" HTTP/1.0\r\n\r\n";
  //AT+CIPSTART="TCP","192.168.20.200",80
  String atTcpPortConnectCmd = "AT+CIPSTART=\"TCP\",\""+ip+"\","+port+"";
  connect_wifi(atTcpPortConnectCmd,1000);
  int len = atHttpGetCmd.length();
  String atSendCmd = "AT+CIPSEND=";
  atSendCmd+=len;
  connect_wifi(atSendCmd,1000);
  connect_wifi(atHttpGetCmd,1000);
}
void checkDistance1() 
{
  digitalWrite(Trigger1Pin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(Trigger1Pin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(Trigger1Pin, LOW);
  Duration1 = pulseIn(Echo1Pin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos
  DistanceCm1 = Duration1 * 10 / 292 / 2;  //convertimos a distancia, en cm
  Serial.print("WaterLevel: ");
  Serial.print(DistanceCm1);
  Serial.println(" cm");
   lcd.setCursor(0,1);
  lcd.print("Level:"); 
  lcd.print(DistanceCm1);
  lcd.print(" cm");
 
}
void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
    lcd.clear();
    lcd.print(cmd);
    Serial.println(cmd);
    Serial1.println(cmd); 
    while(Serial1.available())
    {
      if(Serial1.find("OK"))
      i=8;
    }
    delay(t);
    if(i>5)
    break;
    i++;
  }
  if(i==8)
  {
   Serial.println("OK");
   lcd.setCursor(0,1);
   lcd.print("OK");
  }
  else
  {
   Serial.println("Error");
   lcd.setCursor(0,1);
   lcd.print("Error");
  }
}

