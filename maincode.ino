#include "battery.h"
#include<String.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include "RTClib.h"
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11

SoftwareSerial mySerial(11, 10);  //rx,tx
String textMessage;
RTC_DS3231 rtc;
const byte ROWS = 4;
const byte COLS = 4;
int intML; // store string value of ML in integer
int intLTR; // store string value of LTR in integer
int inttemp; // store string value of temperature in integer
int inthum; // store string value of humidity in integer
int intDelay;
int walvecount = 0;
#define dht_dpin 3
DHT dht(dht_dpin, DHTTYPE); 
Batterylevel bt(A4,1000,100);

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
int timer1_counter;
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {A0, A1, A2, A3};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);  
/******************************
 * Pin declaration
 *****************************/
int motor = 13;         // pump pin
int flowPin = 2;      //This is the input pin on the Arduino
int flowRate = 0;     //This is the value we intend to calculate. 
int watervalve = 4;
volatile int count = 0; //This integer needs to be set as volatile to ensure it updates correctly during the interrupt process.
char delayval[2];
int keypadcount = 0;
String delayinsec = "";
char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
 
int Day; 
int Month;
int Year; 
int Secs;
int Minutes;
int Hours;
 
String dofweek; // day of week
 
String myDate; 
String myTime; 
/******************************
 * Flags
 *****************************/
bool volumeflag = true;
bool delayflag = true;
bool mlvalueflag = true;
bool temphumidflag = true;
bool timeflag = true;
bool LTRvalueflag = true;
bool pumpflag = true;
bool flowdispflag = true;
bool stopflag = true;
bool ltr_select_flag = false;
bool flowflag = false;
bool temphuminputflag = true;
bool huminputflag = true;
bool displayvaluesflag = true;
bool Batteryflag=true;

String val =  ""; // ML value store
String val1 = ""; // LTR value store
String val2 = ""; // time store
String val3 = ""; // time store
String val4 = ""; // time store

int motornoflowcounter = 0;
float h;
float t;
void setup()
{
   bt.begin();
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(flowPin, INPUT);           //Sets the pin as an input
  pinMode(motor,OUTPUT);
  pinMode(watervalve,OUTPUT);
  digitalWrite(motor,LOW);
  dht.begin();
  lcd.backlight();
  lcd.init(); 
  lcd.setCursor(0, 0);
  lcd.print("  **Rohatkee**");
  delay(2000);
  lcd.clear();
//  timertodisplay();
  attachInterrupt(0, flowrate, RISING);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
  
  // Comment out below lines once you set the date & time.
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
    // Following line sets the RTC with an explicit date & time
    // for example to set January 27 2017 at 12:56 you would call:
    // rtc.adjust(DateTime(2017, 1, 27, 12, 56, 0));
  }

  
}

void loop(){
   
//  Serial.println(x);
  mainscreen(); // this function display home screen
   //mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  char customKey = customKeypad.getKey(); // get values from keypad
  if (customKey>0)
  {
    //Serial.print("inside switch case");
    switch (customKey)
    {
      case '1':
         // Serial.println("inside case 1");
          selectvolumetype();
          break;
          
      case '2':
       //   Serial.println("inside case 2");
          selectdelay();
          break;
          
      case '3':
       //   Serial.println("inside case 3");
          ShowTempHumid();
          break;
          
      case '4':
       //   Serial.println("inside case 4");
          ShowTime();
          break;
          
      case '*':
        //  Serial.println("inside case *");
          displayAllValues();
          break;
          
      case 'A':
        //  Serial.println("inside case A");
          displayflowrate();
          break;

      case '#':
           watervalvefunction();
           break;   

      case 'B':
           Batteryfun();
           break;    
         
      default:
          break;
         
    }// end of switch statement
  
  }// end of if statement
 
 while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
  h = dht.readHumidity();
  t = dht.readTemperature();
  
  if(h<=40 && h<inthum){
    displayflowrate();
   }
  if(t>30 && t>inttemp){
    displayflowrate();
  }
}
/**********************************
 * Display Home screen
 *********************************/
void mainscreen()
{
  if(flowflag!=false)
  {
     lcd.clear();
     lcd.setCursor(0, 1);
     lcd.print(flowRate);
     delay(2000);
     flowflag = false;
     lcd.clear();
  }
  val = "";
  val1 = "";
  val2 = "";
  val3 = "";
  val4 = "";
  displayvaluesflag = true;
  temphumidflag = true;
  volumeflag = true;
  timeflag = true;
  volumeflag = true;
  mlvalueflag = true;
  LTRvalueflag = true;
  delayflag = true;
  pumpflag = true;
  flowdispflag = true;
  
  lcd.setCursor(0, 0);
  lcd.print("1Vol");
  lcd.setCursor(0, 1);
  lcd.print("2Delay");
  lcd.setCursor(7, 0);
  lcd.print("3Temp/Hum");
  lcd.setCursor(7, 1);
  lcd.print("4Time");
}

/**********************************
 * Select volume in ML or liters
 *********************************/
int selectvolumetype()
{
  lcd.clear();
 
  while(volumeflag!=false)
  {
  lcd.setCursor(0, 0);
  lcd.print("1 Select ML");
  lcd.setCursor(0, 1);
  lcd.print("2 Select LTR");
  val="";
  val1="";
  char customKey = customKeypad.getKey();
  
  if(customKey == '1')
  {
    //Serial.print("inside ML");
    mlvalueflag = true;
    insertmlvalue();
  }
  else if(customKey == '2'){
    LTRvalueflag = true;
    insertLTRvalue();
  }
  if(customKey == 'B'){
    lcd.clear();
    volumeflag = false;
  } // end of if(customKey == 'B')
  
  }// end of while(volumeflag!=false)
 
}// end of selectvolumetype()

/**********************************
 * Select Delay in miliseconds
 *********************************/
int selectdelay()
{
  lcd.clear();
  
  int cursorcount = 0;
  while(delayflag!=false)
  {
     lcd.setCursor(3, 0);
     lcd.print("Enter Sec.");
     char customKey = customKeypad.getKey();
     if(customKey>0)
     {
   /* if(customKey == 'B'){
      lcd.clear();
      delayflag = false;
      } // end of if(customKey == 'B')*/
    // Serial.println(customKey);
     lcd.setCursor(cursorcount+6, 1);
     lcd.print(customKey);
     cursorcount++;
     val2 = val2+customKey;
   
     if(cursorcount == 3)
     {
      
      intDelay = val2.toInt();
      intDelay = intDelay*200;
      cursorcount = 0;
     /* lcd.setCursor(2, 1);
      lcd.print(String(val2));*/
      Serial.println(intDelay);
      delayflag = false;
            delay(1500);
      lcd.clear();

     }
     //char customKey = customKeypad.getKey(); // get back key stroke
   
     
     }// end of if(customKey>0)
    
  }// end of while(mlvalueflag!=false)
  
}// end of selectdelay()

/**************************
 * Insert volume in ML
 *************************/
int insertmlvalue()
{
  lcd.clear();
  
  int cursorcount = 0;
  while(mlvalueflag!=false)
  {
    
     lcd.setCursor(5, 0);
     lcd.print("ML");
     char customKey = customKeypad.getKey();
     if(customKey>0)
     {
   //  Serial.println(customKey);
     lcd.setCursor(cursorcount, 0);
     lcd.print(customKey);
     cursorcount++;
     val = val+customKey;
    // i++;
     if(cursorcount == 4)
     {
      intML = val.toInt();
      cursorcount = 0;
      lcd.setCursor(2, 1);
      lcd.print(val);
      Serial.println(val);
      
      delay(1000);
      lcd.clear();
      mlvalueflag = false;
     } // end of if(cursorcount == 4)
     //char customKey = customKeypad.getKey(); // get back key stroke
   if(customKey == 'B'){
    lcd.clear();
    mlvalueflag = false;
  } // end of if(customKey == 'B')
     
     }// end of if(customKey>0)
    
  }// end of while(mlvalueflag!=false)
  
}// end of insertmlvalue()


/**************************
 * Insert volume in LTR
 *************************/
int insertLTRvalue(){
  lcd.clear();
  ltr_select_flag = true;
  int i=0;
  int cursorcount = 0;
  while(LTRvalueflag!=false)
  {  
     lcd.setCursor(5, 0);
     lcd.print("LTR");
     char customKey = customKeypad.getKey();
     if(customKey>0)
     {
     //Serial.println(customKey);
     lcd.setCursor(cursorcount, 0);
     lcd.print(customKey);
     cursorcount++;
     val1 = val1+customKey;
     //i++;
     if(cursorcount == 3)
     {
      intML = val1.toInt();
      intML = intML*1000;
      cursorcount = 0;
      lcd.setCursor(2, 1);
      lcd.print(val1);
      Serial.println(intML);
      
      delay(1000);
      lcd.clear();
      LTRvalueflag = false;
     }
     //char customKey = customKeypad.getKey(); // get back key stroke
   if(customKey == 'B'){
    lcd.clear();
    LTRvalueflag = false;
  } // end of if(customKey == 'B')
     
     }// end of if(customKey>0)
    
  }// end of while(LTRvalueflag!=false)
  
}// end of insertLTRvalue()

/************************
 * display temperature and 
 * humidity on LCD
 ***********************/
void ShowTempHumid()
{
  lcd.clear();
  while(temphumidflag!=false)
  {
    val3="";
    val4="";
    temphuminputflag=true;
    huminputflag=true;
   h = dht.readHumidity();
   t = dht.readTemperature();
  lcd.setCursor(0, 0);
  lcd.print("Temp.");
  lcd.setCursor(6, 0);
  lcd.print(t);
  lcd.setCursor(0, 1);
  lcd.print("Humd.");
  lcd.setCursor(6, 1);
  lcd.print(h);
  
  char customKey = customKeypad.getKey(); // get back key stroke
  if(customKey == 'B'){
    lcd.clear();
    temphumidflag = false;
  } // end of if(customKey == 'B')
  else if(customKey == '1'){
    entertemp();
  }
  else if(customKey == '2'){
    enterhum();
  }
  
  }// end of while loop
  
} // end of void ShowTempHumid()

/**************
 * Enter temperature
 ***************/
int entertemp(){
  lcd.clear();
  int cursorcount = 0;
  while(temphuminputflag!=false){
     lcd.setCursor(3, 0);
     lcd.print("C");
     char customKey = customKeypad.getKey();
     if(customKey>0)
     {
      
     //Serial.println(customKey);
     lcd.setCursor(cursorcount, 0);
     lcd.print(customKey);
     cursorcount++;
     val3 = val3+customKey;
     //i++;
     if(cursorcount == 2)
     {
      inttemp  = val3.toInt();
      cursorcount = 0;
      lcd.setCursor(0, 0);
      lcd.print(val3);
      Serial.println(inttemp);
      delay(1000);
      lcd.clear();
      temphuminputflag = false;
     }
     //char customKey = customKeypad.getKey(); // get back key stroke
   /*if(customKey == 'B'){
    lcd.clear();
    temphuminputflag = false;
  } // end of if(customKey == 'B')*/
     
     }// end of if(customKey>0)
    
  }
}

/**************
 * Enter Humidity
 ***************/
int enterhum()
{
  lcd.clear();
  int cursorcount = 0;
  while(huminputflag!=false){
     lcd.setCursor(3, 0);
     lcd.print("%");
     char customKey = customKeypad.getKey();
     if(customKey>0)
     {
      
     //Serial.println(customKey);
     lcd.setCursor(cursorcount, 0);
     lcd.print(customKey);
     cursorcount++;
     val4 = val4+customKey;
     //i++;
     if(cursorcount == 2)
     {
      inthum  = val4.toInt();
      cursorcount = 0;
      lcd.setCursor(0, 0);
      lcd.print(inthum);
      Serial.println(inthum);
      delay(1000);
      lcd.clear();
    huminputflag = false;
     }
     //char customKey = customKeypad.getKey(); // get back key stroke
   /*if(customKey == 'B'){
    lcd.clear();
    huminputflag = false;
  } // end of if(customKey == 'B')*/
     
     }// end of if(customKey>0)
    
  }// end of while loop
}
 
/**************
 * Display date and time 
 ***************/
int ShowTime()
{
 lcd.clear();
 while(timeflag!=false){
   

DateTime now = rtc.now();
        
Day = now.day(); 
Month = now.month(); 
Year = now.year();
Secs = now.second(); 
Hours = now.hour(); 
Minutes = now.minute(); 
dofweek = daysOfTheWeek[now.dayOfTheWeek()];  
 
myDate = myDate +dofweek+ " "+ Day + "/" + Month + "/" + Year ; 
myTime = myTime + Hours +":"+ Minutes +":" + Secs ; 
// send to serial monitor
/*Serial.println(dofweek); 
Serial.println(myDate); 
Serial.println(myTime);*/

//Print on lcd
lcd.setCursor(0, 0);
lcd.print(myDate);
lcd.setCursor(0, 1);
lcd.print(myTime);
myDate = "";   
myTime = ""; 
char customKey = customKeypad.getKey(); // get back key stroke
  if(customKey == 'B')
  {
    lcd.clear();
    timeflag = false;
  } // end of if(customKey == 'B')
//delay(1000);

 }// end of while(timeflag!=false)
}

int displayAllValues()
{
  while(displayvaluesflag !=false){
    
 char customKey = customKeypad.getKey();
 if(customKey == 'C'){
  displayvaluesflag = false;
 }
  Serial.println(val);
  Serial.println(intML);
  Serial.println(intDelay);
  Serial.println(inttemp);
  Serial.println(inthum);
   }
}

float Batteryfun()
{
  lcd.clear();
  float x = bt.calculatelevel();
  
  while(Batteryflag!=false)
  {
    char customKey = customKeypad.getKey();
  if(x>=12.70)
  {
    lcd.setCursor(2,0);
    lcd.print("100%");
  }
  else if(x<=11.0 && x>9.0)
  {
    lcd.setCursor(2,0);
    lcd.print("80%");
  }
  else if(x<=9.0 && x>7.0)
  {
    lcd.setCursor(2,0);
    lcd.print("60%");
  }
  else if(x<=7.0 && x>5.0)
  {
    lcd.setCursor(2,0);
    lcd.print("40%");
  }
  else if(x<=5.0 && x>4.0)
  {
    lcd.setCursor(2,0);
    lcd.print("20%");
  }
  else if(x<=4.0)
  {
    lcd.setCursor(2,0);
    lcd.print("10%");
  }
   if(customKey=='B')
  {
    lcd.clear();
    Batteryflag=false;
  }
  }//end of while loop  while(Batteryflag!=false)
 
}
