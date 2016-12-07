#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESG_AM2315.h>

ESG_AM2315 am2315;
LiquidCrystal_I2C lcd(0x20,16,2);

int LED = 4;
unsigned long desiredLEDon = (unsigned long)1000*60*60*10; // 10 hour
unsigned long desiredLEDoff = (unsigned long)1000*60*60*2; // 2 hour
unsigned long secondUnit = 0;
unsigned long sensorUnit = 0;
bool LEDon = false;

int FAN = 5;
float desiredTem = 20;
float desiredHum = 60;
float dataAM2315[2];
boolean ok;

int PUMP = 6;
unsigned long desiredPUMPon = 20000;
unsigned long PUMPon_reference;
bool PUMPon = false;
bool countPUMPon = false;

void setup() {
  // put your setup code here, to run once:

lcd.init();
Serial.begin(9600);

pinMode(LED, OUTPUT);
pinMode(PUMP, OUTPUT);
pinMode(FAN, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

unsigned long presentTime = millis(); // set time in milliseconds

ok = am2315.readData(dataAM2315);

float currentHUM = dataAM2315[0];
float currentTEM = dataAM2315[1];

if(LEDon)
 {
  if(presentTime - secondUnit >= desiredLEDon)
   {
     secondUnit = presentTime;
     LEDon = false;
     digitalWrite(LED, LOW);
     Serial.println("LEDoff");
     
   }

   if(PUMPon)
   {
     if(countPUMPon)
     {
      if(presentTime - PUMPon_reference <= desiredPUMPon)
      {
        digitalWrite(PUMP, HIGH);
        Serial.println("PUMP on");
      }
      else
      {
        digitalWrite(PUMP, LOW);
      }
     }
   }
 }
 else
 {
  if(presentTime - secondUnit >= desiredLEDoff)
  {
    secondUnit = presentTime;
    LEDon = true;
    digitalWrite(LED, HIGH);
    Serial.print("LEDon");
   
     if(PUMPon)
    {
      PUMPon = false;
    }
    
    else
    {
      PUMPon_reference = presentTime;
      PUMPon = true;

      if(!countPUMPon)
        {
          countPUMPon = true;
        }
       else
       {
          countPUMPon = false;
       }
    }
  }
}

if(presentTime - sensorUnit > 1000)
 {
  if(ok)
  {
    Serial.print("HUM : "); Serial.println(currentHUM);
    Serial.print("TEM : "); Serial.println(currentTEM);
  
    lcd.backlight();
    lcd.display();
    lcd.print("TEMP : ");
    lcd.print(currentTEM);
    lcd.setCursor(0,1);
    lcd.print("HUM  : ");
    lcd.print(currentHUM);

    if((currentHUM > desiredHum) || (currentTEM > desiredTem))
    {
      digitalWrite(FAN, HIGH);
    }
    else
    {
      digitalWrite(FAN, LOW);
    }
  }
  
  else
  {
    Serial.println("Sensor not found, check wiring & pullups!");
    lcd.backlight();
    lcd.display();
    lcd.print("T.T");
  }
 }
}
