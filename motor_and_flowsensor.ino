void flowrate()
{
   count++; 
}
int lcdflow()
{
  while(true)
  {
    lcd.setCursor(2,1);
    lcd.print(flowRate);
  }//end of lcdflow() while loop.
}
int displayflowrate()
{
 lcd.clear();
 char customKey;
 lcd.setCursor(0, 0);
 lcd.print("Flow rate");
 timertodisplay();
 while(flowdispflag!=false)
 {
  h = dht.readHumidity();
  t = dht.readTemperature();
  
  if(intML <= flowRate) // To stop the motor for given delay of time
  {
  //Serial.println("Inside comparison");
  stopmotor();
  //count = 0;
  }
  count = 0;
 // timertodisplay();
  digitalWrite(motor,HIGH);
  digitalWrite(watervalve,HIGH);
  interrupts();   //Enables interrupts on the Arduino
  delay (100);   //Wait 1 second 
  noInterrupts(); //Disable the interrupts on the Arduino
  //Start the math
  flowRate = (count * 2.25);        //Take counted pulses in the last second and multiply by 2.25mL 
  flowRate = flowRate * 60;         //Convert seconds to minutes, giving you mL / Minute
  //lcd.print(flowRate);
  //flowRate = flowRate / 1000;       //Convert mL to Liters, giving you Liters / Minute
 
 if(flowRate == 0){
  motornoflowcounter++;
  Serial.println("flow counter");
  Serial.println(motornoflowcounter);
  if(motornoflowcounter == 200){
     motornoflowcounter = 0;
    timertodisplay();
    count=0;
    flowflag = true;
    digitalWrite(motor,LOW);
    //digitalWrite(watervalve,LOW);
   
    flowdispflag = false;
  }
 }
 else if(flowRate > 0){
  motornoflowcounter = 0;
 }
  Serial.println(flowRate);
 // delay(50);
  char customKey = customKeypad.getKey(); // get back key stroke
  if(customKey == 'B')
  {
    motornoflowcounter = 0;
    timertodisplay();
    count=0;
    flowflag = true;
    //digitalWrite(motor,LOW);
    digitalWrite(watervalve,LOW);
   
    flowdispflag = false;
  } // end of if(customKey == 'B')
  }// end of while loop

if (mySerial.available()>0)
 {
    mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
    textMessage = mySerial.readString();
    Serial.println("Motor set to OFF"); 
    Serial.print(textMessage);
    
    if(textMessage.indexOf("A")>=0){
    // Turn on relay and save current state
    flowdispflag = false;
    Serial.println("Motor set to OFF");  
    textMessage = "";   
  }
 }// end of if (mySerial.available()>0)
  
}
/***************************
 * stop the motor 
 **************************/
void stopmotor(){
 for(int i=0;i<intDelay;i++){
  //digitalWrite(motor,LOW);
  digitalWrite(watervalve,LOW);
  delay(1000);
 }
}

void resetallsettingandstop()
{
  //digitalWrite(motor,LOW);
  digitalWrite(watervalve,LOW);
  val = "";
  val1 = "";
  val2 = "";
  
}
void timertodisplay(){
   noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_counter to the correct value for our interrupt interval
  //timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz
  //timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz
  timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
  
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}
/***************************************
 * Interrupt service routine
 *************************************/
ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = timer1_counter;   // preload timer
  //Serial.println("flowRate");
  lcd.setCursor(2,1);
    lcd.print(flowRate);
  if(flowdispflag!= false)
  {
  /* lcd.setCursor(0, 0);
   lcd.print("1 SV");*/
  }
  
}

int watervalvefunction()
{
  digitalWrite(watervalve,HIGH);
  walvecount++;
  if(walvecount==2)
  {
    digitalWrite(watervalve,LOW);
    walvecount=0;
  }
  Serial.print(walvecount);
}
