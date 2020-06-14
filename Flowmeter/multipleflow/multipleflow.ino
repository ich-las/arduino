#include <SPI.h>
#include <ESP8266WiFi.h> //esp8266 library
#include <ESP8266mDNS.h>
#include <BlynkSimpleEsp8266.h> //blynk library
#include <WiFiUdp.h>  
#include <ArduinoOTA.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library

#ifndef STASSID
#define STASSID "CNI Indonesia"
#define STAPSK  "cni12344321"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
  
//Port Initialization
#define BLYNK_PRINT Serial
//#define TOMBOL 16
#define SENSOR1  D3
#define SENSOR2  D4
#define SENSOR3  D6
#define SENSOR4  D8
#define SENSOR5  3
#define SENSOR6  1

//LCD Definition
#define MODEL ILI9225
#define CS   D0    
#define CD   D2
#define RST  -1
#define LED  -1   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V
//TFT LCD Initialization
LCDWIKI_SPI mylcd(MODEL,CS,CD,RST,LED); //model,cs,dc,reset,led

//LCD Color Definition
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//blynk virtual pin declaration
#define FLOW1 V1
#define FLOW2 V3
#define FLOW3 V5
#define FLOW4 V7
#define FLOW5 V9
#define FLOW6 V11
#define TOT1 V2
#define TOT2 V4
#define TOT3 V6
#define TOT4 V8 
#define TOT5 V10 
#define TOT6 V12

//wifi initialization
char auth[] = "MTddsDPyy9JRV7uGfb1Z9mm9o5fMv6-N";

//led initialization
boolean ledState = LOW;

//calculation variable declaration
int interval = 1000;
long currentMillis = 0;
long previousMillis = 0;

float CF1 = 6.75;
float CF2 = 6.75;
float CF3 = 6.75;
float CF4 = 6.75;
float CF5 = 6.75;
float CF6 = 6.75;

volatile byte PC1,PC2,PC3,PC4,PC5,PC6;
byte PS1,PS2,PS3,PS4,PS5,PS6 = 0;
float FR1,FR2,FR3,FR4,FR5,FR6;
unsigned int FM1,FM2,FM3,FM4,FM5,FM6;
float TM1,TM2,TM3,TM4,TM5,TM6;

//counter subroutine
void IRAM_ATTR pulseCounter1()
{
  PC1++;
}

void IRAM_ATTR pulseCounter2()
{
  PC2++;
}

void IRAM_ATTR pulseCounter3()
{
  PC3++;
}

void IRAM_ATTR pulseCounter4()
{
  PC4++;
}

void IRAM_ATTR pulseCounter5()
{
  PC5++;
}

void IRAM_ATTR pulseCounter6()
{
  PC6++;
}


void setup()
{
  //tft.begin();
  Serial.begin(115200);
  //OTA Initialization
  Serial.println("\nBooting");  
  WiFi.mode(WIFI_STA);  
  WiFi.begin(ssid, password);  
  while (WiFi.waitForConnectResult() != WL_CONNECTED)  
  {  
    Serial.println("Connection Failed! Rebooting...");  
    delay(5000);  
    ESP.restart();  
  }

   ArduinoOTA.onStart([]()  
  {  
    String type;  
    if (ArduinoOTA.getCommand() == U_FLASH)  
    {  
      type = "sketch";  
    }   
    else  
    { // U_SPIFFS  
      type = "filesystem";  
    }  
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()  
    Serial.println("Start updating " + type);  
  });  
  
  ArduinoOTA.onEnd([]() {  
    Serial.println("\nUpdate Finished");  
  });  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {  
    Serial.printf("\nProgress: %u%%\r", (progress / (total / 100)));  
  });  
  ArduinoOTA.onError([](ota_error_t error) {  
    Serial.printf("Error[%u]: ", error);  
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");  
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");  
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");  
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");  
    else if (error == OTA_END_ERROR) Serial.println("End Failed");  
  });  
    
  ArduinoOTA.begin();  
  Serial.println("Ready");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());   
  Serial.print("IP address: ");  
  Serial.println(WiFi.localIP());  
  
  //blynk initialization
  Blynk.begin(auth, ssid, password);
  delay(1000);
  
  //pin mode for LED & Button
  //pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(TOMBOL, INPUT_PULLUP);
    
  
  //Display Init
  mylcd.Init_LCD(); //initialize lcd
  mylcd.Fill_Screen(0xFFFF); //display white

  //pinmode setting for flowmeter
  pinMode(SENSOR1, INPUT_PULLUP);
  pinMode(SENSOR2, INPUT_PULLUP);
  pinMode(SENSOR3, INPUT_PULLUP);
  pinMode(SENSOR4, INPUT_PULLUP);
  pinMode(SENSOR5, INPUT_PULLUP);
  pinMode(SENSOR6, INPUT_PULLUP);

  digitalWrite(SENSOR6,HIGH);
  digitalWrite(SENSOR1,HIGH);
  digitalWrite(SENSOR2,HIGH);
  digitalWrite(SENSOR4,LOW);

  //variable zeroing
  PC1 = 0;
  FR1 = 0.0;
  FM1 = 0;
  TM1 = 0;
    PC2 = 0;
    FR2 = 0.0;
    FM2 = 0;
    TM2 = 0;
      PC3 = 0;
      FR3 = 0.0;
      FM3 = 0;
      TM3 = 0;
        PC4 = 0;
        FR4 = 0.0;
        FM4 = 0;
        TM4 = 0;
          PC5 = 0;
          FR5 = 0.0;
          FM5 = 0;
          TM5 = 0;
            PC6 = 0;
            FR6 = 0.0;
            FM6 = 0;
            TM6 = 0;

//Interrupt Function
  attachInterrupt(digitalPinToInterrupt(SENSOR1), pulseCounter1, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR2), pulseCounter2, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR3), pulseCounter3, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR4), pulseCounter4, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR5), pulseCounter5, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR6), pulseCounter6, FALLING);

  //milis reset
  previousMillis = 0;

}

//Blynk Read to Widget
BLYNK_READ(FLOW1) {Blynk.virtualWrite(FLOW1,FR1*60);}
BLYNK_READ(FLOW2) {Blynk.virtualWrite(FLOW2,FR2*60);}
BLYNK_READ(FLOW3) {Blynk.virtualWrite(FLOW3,FR3*60);}
BLYNK_READ(FLOW4) {Blynk.virtualWrite(FLOW4,FR4*60);}
BLYNK_READ(FLOW5) {Blynk.virtualWrite(FLOW5,FR5*60);}
BLYNK_READ(FLOW6) {Blynk.virtualWrite(FLOW6,FR6*60);}
BLYNK_READ(TOT1) {Blynk.virtualWrite(TOT1,TM1/1000);}
BLYNK_READ(TOT2) {Blynk.virtualWrite(TOT2,TM2/1000);}
BLYNK_READ(TOT3) {Blynk.virtualWrite(TOT3,TM3/1000);}
BLYNK_READ(TOT4) {Blynk.virtualWrite(TOT4,TM4/1000);}
BLYNK_READ(TOT5) {Blynk.virtualWrite(TOT5,TM5/1000);}
BLYNK_READ(TOT6) {Blynk.virtualWrite(TOT6,TM6/1000);}

//flowmeter
void flowmeter()
{
currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    PS1 = PC1;
    PS2 = PC2;
    PS3 = PC3;
    PS4 = PC4;
    PS5 = PC5;
    PS6 = PC6;
    PC1 = 0;
    PC2 = 0;
    PC3 = 0;
    PC4 = 0;
    PC5 = 0;
    PC6 = 0;
    flowcounter();
    }
}

void flowcounter()
{
FR1 = ((1000.0 / (millis() - previousMillis)) * PS1) / CF1;
FR2 = ((1000.0 / (millis() - previousMillis)) * PS2) / CF2;
FR3 = ((1000.0 / (millis() - previousMillis)) * PS3) / CF3;
FR4 = ((1000.0 / (millis() - previousMillis)) * PS4) / CF4;
FR5 = ((1000.0 / (millis() - previousMillis)) * PS5) / CF5;
FR6 = ((1000.0 / (millis() - previousMillis)) * PS6) / CF6;

previousMillis = millis();

FM1 = (FR1 /60) *1000;
TM1 += FM1;
  FM2 = (FR2 /60) *1000;
  TM2 += FM2;
    FM3 = (FR3 /60) *1000;
    TM3 += FM3;
      FM4 = (FR4 / 60) *1000;
      TM4 += FM4;
        FM5 = (FR5 / 60) *1000;
        TM5 += FM5;
          FM6 = (FR6 / 60) *1000;
          TM6 += FM6;
}

//LCD DISPLAY
void IRAM_ATTR lcdshow()
{
  mylcd.Set_Text_Mode(0);
  mylcd.Set_Text_Size(1);
  mylcd.Set_Text_Back_colour(WHITE);
  mylcd.Set_Text_colour(BLACK);
  
  mylcd.Print_String("PLS FLOWRATE  : ", 2,2);
  mylcd.Print_Number_Float(FR1*60,2 ,100 ,2 , '.', 0, ' ');
  mylcd.Print_String("L/H", RIGHT,2);
  mylcd.Print_String("PLS TOTALIZER : ", 2,11);
  mylcd.Print_Number_Int(TM1/1000,100 ,11 ,0, ' ',10);
  mylcd.Print_String("L", RIGHT,11);

  mylcd.Print_String("R1 FLOWRATE   : ", 2,29);
  mylcd.Print_Number_Float(FR2*60,2 ,100 ,29 , '.', 0, ' ');
  mylcd.Print_String("L/H", RIGHT,29);
  mylcd.Print_String("R1 TOTALIZER  : ", 2,38);
  mylcd.Print_Number_Int(TM2/1000,100 ,38 ,0, ' ',10);
  mylcd.Print_String("L", RIGHT,38);  

  mylcd.Print_String("STC FLOWRATE  : ", 2,56);
  mylcd.Print_Number_Float(FR3*60,2 ,100 ,56 , '.', 0, ' ');
  mylcd.Print_String("L/H", RIGHT,56);
  mylcd.Print_String("STC TOTALIZER : ", 2,65);
  mylcd.Print_Number_Int(TM3/1000,100 ,65 ,0, ' ',10);
  mylcd.Print_String("L", RIGHT,65);  

  mylcd.Print_String("ENI FLOWRATE  : ", 2,83);
  mylcd.Print_Number_Float(FR4*60,2 ,100 ,83 , '.', 0, ' ');
  mylcd.Print_String("L/H", RIGHT,83);
  mylcd.Print_String("ENI TOTALIZER : ", 2,92);
  mylcd.Print_Number_Int(TM4/1000,100 ,92 ,0, ' ',10);
  mylcd.Print_String("L", RIGHT,92);  

  mylcd.Print_String("NHP FLOWRATE  : ", 2,110);
  mylcd.Print_Number_Float(FR5*60,2 ,100 ,110 , '.', 0, ' ');
  mylcd.Print_String("L/H", RIGHT,110);
  mylcd.Print_String("NHP TOTALIZER : ", 2,119);
  mylcd.Print_Number_Int(TM5/1000,100 ,119 ,0, ' ',10);
  mylcd.Print_String("L", RIGHT,119);  

  mylcd.Print_String("R2 FLOWRATE  : ", 2,137);
  mylcd.Print_Number_Float(FR6*60,2 ,100 ,137 , '.', 0, ' ');
  mylcd.Print_String("L/H", RIGHT,137);
  mylcd.Print_String("R2 TOTALIZER : ", 2,146);
  mylcd.Print_Number_Int(TM6/1000,100 ,146 ,0, ' ',10);
  mylcd.Print_String("L", RIGHT,146);  
}

//main program
void loop()
{
  ArduinoOTA.handle();
  Blynk.run();
  lcdshow();
  flowmeter();
  
}
