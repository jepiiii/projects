
#include <ezButton.h> // include ezButton lib
#include <Keyboard.h> // include Keyboard lib
#include <LiquidCrystal_I2C.h> // include LCD I2C lib
#include <TinyRF_RX.h>
const int switchState = 7; // switch pin 7
const int TX_LED = 30; // tx-led pin 30 (in built led)
// int counter = 0; 
int pageNumber = 0; //current preset number 1 to 3
const int presetAmount = 2; // 3 presets
boolean change = false;
boolean change2 = false;
ezButton button1(9);
ezButton button2(10);
LiquidCrystal_I2C lcd(0x27, 16, 2);
boolean moreThanTen = false;
uint32_t previousTime = 0;
unsigned int pressAmount = 0;
String pageDesc[3] = {
  "osu!",
  "Spam",
  "CopyNPaste"
};
boolean runOnceCase1 = false;
boolean runOnceTick = false;

uint8_t rxPin = 5;

void setup(){
  Serial.begin(115200);
  //make sure you call this in your setup
  setupReceiver(rxPin);
  // Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  //lcd.setCursor(0, 0);
  //lcd.print("KeyCount");

  pinMode(TX_LED, OUTPUT);
  pinMode(switchState, INPUT_PULLUP);
  // put your setup code here, to run once:
  // make pin 2 an input and turn on the
  // pullup resistor so it goes high unless
  // connected to ground:
  // Serial.begin(9600);
  Keyboard.begin();
  button1.setDebounceTime(10);
  button2.setDebounceTime(10);

}

void receive(){
  const uint8_t bufSize = 30;
  byte buf[bufSize];
  uint8_t numLostMsgs = 0;
  uint8_t numRcvdBytes = 0;

  // number of received bytes will be put in numRcvdBytes
  // if sequence numbering is enabled the number of lost messages will be put in numLostMsgs
  // if you have disabled sequence numbering or don't need number of lost messages you can omit this argument
  uint8_t err = getReceivedData(buf, bufSize, numRcvdBytes, numLostMsgs);

  // the receiver has a circular FIFO buffer
  // if getReceivedData() isn't called frequently enough then older messages will get overwritten
  // so make sure the frequency at which you send messages in your tx code is slower than the frequency
  // at which you call getReceivedData() in your rx code to prevent that
  // specially when you are using sendMulti()
  // duplicate messages that are sent using sendMulti() will stay in the buffer until you read the first one
  // you can change the buffer size in settings.h

  if(err == TRF_ERR_NO_DATA){
    return;
  }

  if(err == TRF_ERR_BUFFER_OVERFLOW){
    Serial.println("Buffer too small for received data!");
    return;
  }

  if(err == TRF_ERR_CORRUPTED){
    Serial.println("Received corrupted data.");
    return;
  }

  // if sequence numbering is enabled and you use the sendMulti() function for sending a message
  // multiple times then getReceivedData() will return TRF_ERR_SUCCESS only once
  // all the duplicate messages will be automatically ignored
  // this means all you need to do is check if the return code is TRF_ERR_SUCCESS
  // these are non-repeated, crc-valid messages
  if(err == TRF_ERR_SUCCESS){

    Serial.print("Received: ");

    for(int i=0; i<numRcvdBytes; i++){
      Serial.print((char)buf[i]);
    }

    Serial.println("");

    if(numLostMsgs>0){
      Serial.print(numLostMsgs);
      Serial.println(" messages were lost before this message.");
    }

  }
  
}




// <------------------ For Testing ------------------->
//bool isNumber(String s)
//        {
//          for (int i = 0; i < s.length(); i++)
//              if (isdigit(s[i]) == false)
//                  return false;
//
//          return true;
//        }
// <------------------ For Testing ------------------->        



void kps(boolean isPressed) {
  if(isPressed) {
    //Serial.println(millis() - previousTime <= 1 * 1000 ? "True" : "False");
    if (millis() - previousTime <= 1 * 1000) {     
      pressAmount++;      
    }
  //Calculate the amount of keys pressed per second
  }  
}

//int recordedTotal[10] = {};
void wait(){
  int period = 1000;
  unsigned long time_now = 0;
  
  time_now = millis();
   
    //wait 1 s
   
    while(millis() < time_now + period){
        //wait approx. [period] ms
    }
}
int bpm(){
  String bpm = String(round((pressAmount *60)/4));
    lcd.setCursor(6, 0);
    lcd.print("BPM:"); //1/4 bpm
      switch(bpm.length()){
        case 1:
          lcd.setCursor(6, 1);
          wait();
          lcd.print("00" + bpm); //1/4 bpm
            break;

        case 2:
          lcd.setCursor(6, 1);
          wait();
          lcd.print("0" + bpm); //1/4 bpm
            break;

        case 3:
          lcd.setCursor(6, 1);
          wait();
          lcd.print(bpm); //1/4 bpm
            break;   
      }
  }

void tick(int preset) {
  if(!runOnceTick) {
    previousTime = millis();
    runOnceTick = true;
  }
  switch (preset) {
    case 0:
    if (!runOnceCase1) {
      lcd.setCursor(0,0);
      lcd.print("KPS:");
      lcd.setCursor(0, 1);
      lcd.print("00"); 
      lcd.setCursor(0, 0);
      lcd.print(previousTime);
      runOnceCase1 = true;
    }
    if (millis() - previousTime >= 1 * 1000) { 
 previousTime = millis();
 String temp = String(pressAmount);//Convert pressAmount to string
 if(temp.length() == 1) { //Check if temp is 1 digit long
   temp = "0" + temp;
 }
 //lcd.print(temp); //This will display the total    
 bpm(); 
 lcd.setCursor(0, 0);
 lcd.print("KPS:");
 lcd.setCursor(0,1);
 lcd.print(temp);
 previousTime = millis();   
 pressAmount = 0;   //This will reset the Amount of keys pressed in the past second
  break;
    }
  }
}
  
  



// btn1 Function accept args "PresetNumber" and "isPressed" Boolean 
void btn1(int preset, boolean isPressed) {
  //if (isPressed) is true, run the ff:
  if (isPressed) {
    switch (preset)  {
      case 0:
        Keyboard.press(72);
        kps(true)
        ;
        
        // code to be executed if 
        // expression is equal to constant1;
        break;

      case 1:
        Keyboard.print("aaaa");
        // code to be executed if
        // expression is equal to constant2;
        break;
      case 2:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press('c');
        break;
    }
  //if (isPressed) is false, run the ff
  } else {
    switch (preset)  {
      case 0:
        Keyboard.release(72);
        // code to be executed if 
        // expression is equal to constant1;
        break;

      case 1:
        // code to be executed if
        // expression is equal to constant2;
        break;
      case 2:
        Keyboard.releaseAll();
      }
  }
}

void btn2(int preset, boolean isPressed) {
  if (isPressed) {
    switch (preset)  {
      case 0:
        Keyboard.press(75);
        kps(true);
        // code to be executed if 
        // expression is equal to constant1;
        break;

      case 1:
        Keyboard.print("japanese goblin");
        // code to be executed if
        // expression is equal to constant2;
        break;

      case 2:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press('v');
    }
  } else {
    switch (preset)  {
      case 0:
        Keyboard.release(75);
      break;
      
      case 2:
          Keyboard.releaseAll();
          // code to be executed if 
          // expression is equal to constant1;
          break;
    }
  }
}

// void btnInit(int preset) {
//   switch (preset)  {
//     case 0:
//       break;
//     case 1:
//       break;
//     case 2:
//       break;
//   }
// }


void redirect(int pageNumber){
 // int lcdLength = 16;
 // int offset = lcdLength - pageDesc[pageNumber].length();
    switch(pageNumber){
      case 0:
        lcd.setCursor(12, 1);
        lcd.print("osu!");
      break;
      case 1:
        lcd.setCursor(12, 1);
        lcd.print("SPAM");
      break;
      case 2:
        lcd.setCursor(10, 1);
        lcd.print("CPYPST");
      break;
    }
}
void loop()
{
  receive();

  //lcd.setCursor(0, 1);
  button1.loop(); // MUST call the loop() function first
  button2.loop(); // MUST call the loop() function first

  if (digitalRead(switchState) == LOW) //On
  {
    if (!change) {
      runOnceCase1 = false;
      change2 = false;
      change = true;
      lcd.clear();
      lcd.setCursor(7,0);
      lcd.print(pageNumber + 1);

      int x = 0;
      String currentDesc = pageDesc[pageNumber];
      if(currentDesc.length() > 16) {
        currentDesc = currentDesc.substring(0, 12) + "...";
      }
      //Center currentDesc text
      x = (15 - currentDesc.length()) / 2;
      lcd.setCursor(x,1);
      lcd.print(pageDesc[pageNumber]);
    }
    if (button1.isPressed()) //preset select left
    {
      if (pageNumber == 0) {
        pageNumber = presetAmount;
      } else {
        pageNumber--;
      }
      //Serial.println(pageNumber);
      //lcd.print(pageNumber);
      change = false;
    }
    if (button2.isPressed()) //preset select left
    {
      if (pageNumber == presetAmount) {
        pageNumber = 0;
      } else {
        pageNumber++;
      }
     // Serial.println(pageNumber);
      //lcd.print(pageNumber);
      change = false;
    }
  } 
  else {
    tick(pageNumber);
    redirect(pageNumber);
    if (!change2) {
      change2 = true;
      change = false;
      lcd.clear();
      lcd.setCursor(15, 0);
      lcd.print(pageNumber + 1);
      lcd.setCursor(15, 0);
       //if switch is off put page number on top right
      //btnInit(pageNumber);
    }
    digitalWrite(TX_LED, HIGH);
    if (button1.isPressed()) {
      btn1(pageNumber, true);
    };
    if (button1.isReleased()) {
      btn1(pageNumber, false);
    };

    if (button2.isPressed()) {
      btn2(pageNumber, true);
    };
    if (button2.isReleased()) {
      btn2(pageNumber, false);
    };
  }

  
                    /*
                      if (button1.isPressed())
                      {
                        if (digitalRead(switchState) == HIGH)
                        {
                          Keyboard.press(72);
                          lcd.print(counter = counter + 1);
                        }
                        else {
                          Keyboard.print("aaaa");
                        }
                      }

                      if (button1.isReleased())
                        Keyboard.release(72);
                      // Serial.println("The button 1 is released");

                      if (button2.isPressed())
                      {
                        if (digitalRead(switchState) == HIGH)
                        {
                          Keyboard.press(75);
                          lcd.print(counter = counter + 1);
                        }
                        else {
                          Keyboard.print("japanese goblin");
                        }
                      }

                      if (button2.isReleased())
                        Keyboard.release(75);
                      // Serial.println("The button 2 is released"); */
} // end of void loop

// Keyboard.write(75); }
/*
void btn1(preset) {
  switch (preset)  {
    case 1:
        Keyboard.press(72);
        // code to be executed if 
        // expression is equal to constant1;
        break;

    case 2:
        Keyboard.print("aaaa");
        // code to be executed if
        // expression is equal to constant2;
        break;
  }
}

void btn2(preset) {
  switch (preset)  {
    case 1:
        Keyboard.press(72);
        // code to be executed if 
        // expression is equal to constant1;
        break;

    case 2:
        Keyboard.print("japanese goblin");
        // code to be executed if
        // expression is equal to constant2;
        break;
  }
}*/
/*
If switch on
_____________________________________
|                                   |
|                                   |
|                                   |
|                                   |
|                pageNumber                   |
|           Title                        |
|                                   |
|                                   |
|                                   |
|                                   |
-------------------------------------

If switch off
_____________________________________
|                                   |
|                              1     |
|                                   |
|                                   |
|                Counter                   |
|                                   |
|                                   |
|                                   |
|                                   |
|                                   |
-------------------------------------*/
