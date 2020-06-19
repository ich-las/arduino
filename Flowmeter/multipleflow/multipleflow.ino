#include <SPI.h>
#define BLYNK_TIMEOUT_MS  500  // must be BEFORE BlynkSimpleEsp8266.h doesn't work !!!
#define BLYNK_HEARTBEAT   17   // must be BEFORE BlynkSimpleEsp8266.h works OK as 17s
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <BlynkSimpleEsp32_SSL.h>
#include <Adafruit_GFX.h>
#include <ILI9225_kbv.h>
#include <Update.h>

//wifi initialization
//char auth[] = "aU1Tw08w1Uq77C91FGtPJsm9l-dwDY79";
char auth[] = "bSCmZrq2uhfogScohN1dWkYyW2iBjYvR";
//const char* server = "blynk.ichlas.ga";
unsigned int port = 32770;                         
const char* host = "esp32";
char ssid[] = "CNI Indonesia";
char pass[] = "cni12344321";

//webserver
WebServer server(80);

/*
 * Login page
 */

const char* loginIndex = 
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Username:</td>"
        "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";
 
/*
 * Server Index Page
 */
 
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";
  
//Port Initialization
#define BLYNK_PRINT Serial

//#define TOMBOL 16
#define SENSOR1  32
#define SENSOR2  33
#define SENSOR3  12
#define SENSOR4  26
#define SENSOR5  27
#define SENSOR6  14

//LCD Definition
ILI9225_kbv tft(22, 17, 5);

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

//blynk timer
BlynkTimer timer;
bool on = 0;
bool online = 0;

//calculation variable declaration
int interval = 500;
long currentMillis = 0;
long previousMillis = 0;

float CF1 = 7;
float CF2 = 7;
float CF3 = 7;
float CF4 = 7;
float CF5 = 7;
float CF6 = 7;

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
  //serial initialization
  Serial.begin(115200);

  //LCD Initialization
  tft.begin();
  delay(1000);
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  testTriangles();
  testFilledTriangles();
  tft.fillScreen(WHITE);
    
  //OTA Initialization
  Serial.println("\nBooting");  
  WiFi.mode(WIFI_STA);
  //Blynk.begin(auth);
  Blynk.config(auth);
  Blynk.connect();
  CheckConnection();
  timer.setInterval(5000L, CheckConnection); 
  timer.setInterval(3000L, myTimerEvent); 
 
  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
  Serial.println("Ready");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());   
  Serial.print("IP address: ");  
  Serial.println(WiFi.localIP());  
   
  //pinmode setting for flowmeter
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);
  pinMode(SENSOR3, INPUT);
  pinMode(SENSOR4, INPUT);
  pinMode(SENSOR5, INPUT);
  pinMode(SENSOR6, INPUT);

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
  attachInterrupt(digitalPinToInterrupt(SENSOR1), pulseCounter1, RISING);
  attachInterrupt(digitalPinToInterrupt(SENSOR2), pulseCounter2, RISING);
  attachInterrupt(digitalPinToInterrupt(SENSOR3), pulseCounter3, RISING);
  attachInterrupt(digitalPinToInterrupt(SENSOR4), pulseCounter4, RISING);
  attachInterrupt(digitalPinToInterrupt(SENSOR5), pulseCounter5, RISING);
  attachInterrupt(digitalPinToInterrupt(SENSOR6), pulseCounter6, RISING);

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
FR1 = ((1000.0 / (millis() - previousMillis)) * PS1) / CF1 ;
FR2 = ((1000.0 / (millis() - previousMillis)) * PS2) / CF2 ;
FR3 = ((1000.0 / (millis() - previousMillis)) * PS3) / CF3 ;
FR4 = ((1000.0 / (millis() - previousMillis)) * PS4) / CF4 ;
FR5 = ((1000.0 / (millis() - previousMillis)) * PS5) / CF5 ;
FR6 = ((1000.0 / (millis() - previousMillis)) * PS6) / CF6 ;

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
void lcdshow()
{
  tft.setTextSize(1);
  tft.setTextColor(BLACK,WHITE);
  tft.setCursor(0,0);
  tft.println("");
  
  tft.print("PLS FLOWRATE : ");
  tft.println(FR1*60);
  tft.setCursor(140,8);
  tft.println("L/H");
  tft.print("PLS TOTALIZER: ");
  tft.println(TM1/1000);
  tft.setCursor(140,16);
  tft.println("L");
  tft.println(""); 
  
  tft.print("R1 FLOWRATE  : ");
  tft.println(FR2*60);
  tft.setCursor(140,32);
  tft.println("L/H");
  tft.print("R1 TOTALIZER : ");
  tft.println(TM2/1000);
  tft.setCursor(140,40);
  tft.println("L");
  tft.println("");  

  tft.print("STC FLOWRATE : ");
  tft.println(FR3*60);
  tft.setCursor(140,56);
  tft.println("L/H");
  tft.print("STC TOTALIZER: ");
  tft.println(TM3/1000);
  tft.setCursor(140,64);
  tft.println("L");
  tft.println("");  

  tft.print("ENI FLOWRATE : ");
  tft.println(FR4*60);
  tft.setCursor(140,80);
  tft.println("L/H");
  tft.print("ENI TOTALIZER: ");
  tft.println(TM4/1000);
  tft.setCursor(140,88);
  tft.println("L");  
  tft.println("");

  tft.print("NHP FLOWRATE : ");
  tft.println(FR5*60);
  tft.setCursor(140,104);
  tft.println("L/H");
  tft.print("NHP TOTALIZER: ");
  tft.println(TM5/1000);
  tft.setCursor(140,112);
  tft.println("L"); 
  tft.println(""); 

  tft.print("R2 FLOWRATE  : ");
  tft.println(FR6*60);
  tft.setCursor(134,128);
  tft.println(" L/H");
  tft.print("R2 TOTALIZER : ");
  tft.println(TM6/1000);
  tft.setCursor(134,136);
  tft.println(" L");  
  tft.println("");
}

void testTriangles() {
    int           n, i, cx = tft.width()  / 2 - 1,
                        cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    n     = min(cx, cy);
    for (i = 0; i < n; i += 5) {
        tft.drawTriangle(
            cx    , cy - i, // peak
            cx - i, cy + i, // bottom left
            cx + i, cy + i, // bottom right
            tft.color565(0, 0, i));
    }
}

void testFilledTriangles() {
    int           i, cx = tft.width()  / 2 - 1,
                     cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    for (i = min(cx, cy); i > 10; i -= 5) {
        tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                         tft.color565(0, i, i));
        tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                         tft.color565(i, i, 0));
    }
}

void CheckConnection(){    // check every 11s if connected to Blynk server
  if(!Blynk.connected()){
    online = 0;
    yield();
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Not connected to Wifi! Connect...");
      WiFi.begin(ssid, pass);
      delay(1000); //give it some time to connect
      if (WiFi.status() != WL_CONNECTED)
      {
        Serial.println("Cannot connect to WIFI!");
        online = 0;
      }
      else
      {
        Serial.println("Connected to wifi!");
      }
    }
    
    if ( WiFi.status() == WL_CONNECTED && !Blynk.connected() )
    {
      Serial.println("Not connected to Blynk Server! Connecting..."); 
      Blynk.connect();  // // It has 3 attempts of the defined BLYNK_TIMEOUT_MS to connect to the server, otherwise it goes to the enxt line 
      if(!Blynk.connected()){
        Serial.println("Connection failed!");
        online = 0;
      }
      else
      {
        online = 1;
      }
    }
  }
  else{
    Serial.println("Connected to Blynk server!"); 
    online = 1;    
  }
}


void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  if (online == 1)
  {
    Blynk.virtualWrite(V40, millis() / 1000);    
  }
  else 
  {
    Serial.println("Working Offline!");  
  }
  
  if (on == 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)    
    on = 1;
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW                  
    on = 0;
  }
  Serial.println(millis() / 1000);
}

//main program
void loop()
{
  server.handleClient();
  if(Blynk.connected()){Blynk.run();}
  timer.run();
  lcdshow();
  flowmeter();
}
