#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimerOne.h>
#include <EEPROM.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

//------Pins-------------------------------------------
int pinJoyX = 0;
int pinJoyY = 1;
int pinJoyBtn = 2;
int pinRelay1 = 3;
int pinRelay2 = 4;
int pinRelay3 = 5;
int pinRedLED = 6;
int pinGreenLED = 7;

//------Joy Stick Values-------------------------------
int joyX = 0;
int joyY = 0;
int joyBtn = 0;
int btnState = 0;
int lastBtnState = 0;
int joyXState = 0;
int joyXLastState = 0;
int joyYState = 0;
int joyYLastState = 0;

//------Drink Menu-------------------------------------
char* menu[] = {"Drink1", "Drink2", "Drink3", "Drink4", "Drink5", "Drink6"};

//------Selection Values-------------------------------
int drinkSel = 0;
int sizeSel = 0;
int settingSel = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(pinJoyBtn, INPUT_PULLUP);
  pinMode(pinRedLED, OUTPUT);
  pinMode(pinGreenLED, OUTPUT);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("DrinkMaker   v2.0");
  display.display();
  delay(2000);
  display.clearDisplay();
  Timer1.initialize(500000);
  updateDrinkSelScreen();
}

void loop() {
  // put your main code here, to run repeatedly:
  updateDrinkSelScreen();
  joyX = analogRead(pinJoyX);
  joyY = analogRead(pinJoyY);
//  Serial.print("X: ");
//  Serial.print(joyX);
//  Serial.print(" Y: ");
//  Serial.print(joyY);
//  Serial.print(" ");
//  Serial.println(btnState);
  delay(50);
  btnState = digitalRead(pinJoyBtn);
  if(btnState != lastBtnState){
    if(btnState == HIGH){
      Serial.println("on");
    } else {
      Serial.println("off");
    }
    delay(50);
  }
  lastBtnState = btnState;

  if(joyX > 800 || joyX < 300) joyXState = 1;
  else joyXState = 0;
  if(joyY > 800 || joyY < 300) joyYState = 1;
  else joyYState = 0;

  if(joyXState != joyXLastState){
    if(joyXState == 1 && joyX > 800){
      joyRight();  
    } else if(joyXState == 1 && joyX < 300){
      joyLeft();
    }
    joyXLastState = joyXState;
  }
  if(joyYState != joyYLastState){
    if(joyYState == 1 && joyY > 800){
      joyDown();
    } else if(joyYState == 1 && joyY < 300){
      joyUp();
    }
    joyXLastState = joyXState;
  }
}

void updateDrinkSelScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Drink ");
  display.print(drinkSel + 1);
  display.display();
}

void joyLeft(){
  if(drinkSel == 0) drinkSel = sizeof(menu) / sizeof(char*) - 1;
  else drinkSel--;
  Serial.println("Drink " + drinkSel);
  updateDrinkSelScreen();
}

void joyRight(){
  if(drinkSel == sizeof(menu) / sizeof( char* ) - 1) drinkSel = 0;
  else drinkSel++;
  Serial.println("Drink " + drinkSel);
  updateDrinkSelScreen();
}

void joyDown(){
  Serial.println("Start");
  joyXLastState = 0;
  joyYLastState = 0;
  display.clearDisplay();
  display.display();
  for(;;){
    joyX = analogRead(pinJoyX);
    joyY = analogRead(pinJoyY);
//    Serial.print("X: ");
//    Serial.print(joyX);
//    Serial.print(" Y: ");
//    Serial.print(joyY);
//    Serial.print(" ");
//    Serial.println(btnState);
    if(joyX > 800 || joyX < 300) joyXState = 1;
    else joyXState = 0;
    if(joyY > 800 || joyY < 300) joyYState = 1;
    else joyYState = 0;
    if(joyYState != joyYLastState){
      if(joyYState == 1 && joyY > 800){
        joyDown();
      } else if(joyYState == 1 && joyY < 300){
        break;
      }
      joyYLastState = joyYState;
    }
  }
  Serial.println("Done");
}

void joyUp(){
  
}

