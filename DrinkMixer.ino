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
int pinRelay[] = {3, 4, 5};
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
//------Pump Menu--------------------------------------
int pump[] = {0, 0, 0};
int changes[] = {0, 0, 0};

//------Selection Values-------------------------------
int drinkSel = 0;
int menuSel = 0; //Determines whether to display the drink menu or setting menu
int pumpSel = 0; //0-2


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
  Serial.println(menuSel);
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
  if(menuSel == 0){
    updateDrinkSelScreen();
    if(btnState != lastBtnState){
      if(btnState == HIGH){
        pourDrink();
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
        drinkRight();  
      } else if(joyXState == 1 && joyX < 300){
        drinkLeft();
      }
      joyXLastState = joyXState;
    }
    if(joyYState != joyYLastState){
      if(joyYState == 1 && joyY > 800){
        getPumpValues();
        pumpSel = 0;
        menuSel = 1;
      } else if(joyYState == 1 && joyY < 300){
        //DrinkJoyUp
      }
      joyYLastState = joyYState;
    }
  } else if(menuSel == 1){  //==============Setting Menu Control========================
    updateSettingScreen();
    if(btnState != lastBtnState){
      if(btnState == HIGH){
        setPumpChanges();
        menuSel = 0;
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
        pumpRight();  
      } else if(joyXState == 1 && joyX < 300){
        pumpLeft();
      }
      joyXLastState = joyXState;
    }
    if(joyYState != joyYLastState){
      if(joyYState == 1 && joyY > 800){
        pumpDown();
      } else if(joyYState == 1 && joyY < 300){
        pumpUp();
      }
      joyYLastState = joyYState;
    }
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

void updateSettingScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Pump ");
  display.println(pumpSel + 1);
  display.print(changes[pumpSel]);
  display.print(" oz");
  display.display();
}

void pourDrink(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("Pouring");
  display.println("Drink...");
  getPumpValues();
  display.display();
  for(int i = 0; i < 3; i++){
    analogWrite(pinRelay[i], 0);
    delay(pump[i] * 1000); //Change this to use timer1
    analogWrite(pinRelay[i], 255);
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Drink");
  display.println("Complete");
  display.display();
  delay(2000);
}

void drinkLeft(){
  if(drinkSel == 0) drinkSel = sizeof(menu) / sizeof(char*) - 1;
  else drinkSel--;
  updateDrinkSelScreen();
}

void drinkRight(){
  if(drinkSel == sizeof(menu) / sizeof( char* ) - 1) drinkSel = 0;
  else drinkSel++;
  updateDrinkSelScreen();
}

void getPumpValues(){
  for(int i = 0; i < 3; i++){
    pump[i] = EEPROM.read(drinkSel * 3 + i);
    changes[i] = EEPROM.read(drinkSel * 3 + i);
  }
}

void setPumpChanges(){
  for(int i = 0; i < 3; i++){
    if(pump[i] != changes[i]){
      EEPROM.write(drinkSel * 3 + i, changes[i]);
    }
  }
}

void pumpLeft(){
  if(pumpSel == 0) pumpSel = sizeof(changes) / sizeof( int ) - 1;
  else pumpSel--;
  updateSettingScreen();
}

void pumpRight(){
  if(pumpSel == sizeof(changes) / sizeof( int ) - 1) pumpSel = 0;
  else pumpSel++;
  updateSettingScreen();
}

void pumpDown(){
  if(changes[pumpSel] == 0) changes[pumpSel] = 10;
  else changes[pumpSel]--;
  updateSettingScreen();
}

void pumpUp(){
  if(changes[pumpSel] == 10) changes[pumpSel] = 0;
  else changes[pumpSel]++;
  updateSettingScreen();
}

