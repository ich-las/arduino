#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define LED_BUILTIN  2
#define SENSOR  14
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 6.75;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
float totalMilliLitres;
unsigned int frac;
unsigned int totalfrac;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup()
{
  Serial.begin(9600);
  delay(10);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  delay(2000);
  display.clearDisplay();
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR, INPUT_PULLUP);
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
}

void loop()
{
  currentMillis = millis();
if (currentMillis - previousMillis > interval) {
    pulse1Sec = pulseCount;
    pulseCount = 0;
// Because this loop may not complete in exactly 1 second intervals we calculate
// the number of milliseconds that have passed since the last execution and use
// that to scale the output. We also apply the calibrationFactor to scale the output
// based on the number of pulses per second per units of measure (litres/minute in
// this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
    frac = (flowRate - int(flowRate)) * 10;
// Divide the flow rate in litres/minute by 60 to determine how many litres have
// passed through the sensor in this 1 second interval, then multiply by 1000 to
// convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
// Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    totalfrac = (totalMilliLitres - int(totalMilliLitres)) * 10;
  }

  // Print the flow rate for this second in litres / minute
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Flow rate: ");
    display.print(int(flowRate));  // Print the integer part of the variable
    display.print(".");
    display.print(frac,DEC);
    display.println(" L/min");
    display.println("Totalizer: ");
    display.print(totalMilliLitres / 1000);
    display.print(" L");
    display.display();

  //blink led indicator
  if (flowRate > 0) {
    blinkled;}
    else
    {digitalWrite(LED_BUILTIN, LOW);
  }
}

void blinkled()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
