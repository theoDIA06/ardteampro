#define USE_ARDUINO_INTERRUPTS true
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "PulseSensorPlayground.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "avr/wdt.h"

LiquidCrystal_I2C lcd(0x3F, 20, 4);
void printDetail(uint8_t type, int value);
int avg;
int incbpm;
const int pulse = 0;
int Threshold = 220;
int sw_state = 0;
int sw_pre_state = 0;
int i = 0;
SoftwareSerial mySoftwareSerial(2, 3); // RX, TX
DFRobotDFPlayerMini music;
PulseSensorPlayground ps;

void reset() {
  wdt_enable(WDTO_15MS);
  while (1) {}
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hi");
  lcd.setCursor(0, 1);
  lcd.print("if you wanna start?");
  lcd.setCursor(0, 2);
  lcd.print("place your hand");
  lcd.setCursor(0, 3);
  lcd.print("on the sensor");
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);
  Serial.println("DFPlayer Mini test!");
  ps.analogInput(pulse);
  ps.setThreshold(Threshold);
  pinMode(13, INPUT);

  if (!music.begin(mySoftwareSerial, true, true)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1. Please Recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(1);
  }

   if (ps.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }

  Serial.println(F("DFPlayer Mini online."));

  music.volume(30);  // Set volume level (0~30).
}

void loop() {
  if (ps.sawStartOfBeat()) {
    lcd.clear();
    int bpm = ps.getBeatsPerMinute();
    lcd.setCursor(0, 0);
    lcd.print("BPM : ");
    lcd.setCursor(6, 0);
    lcd.print(bpm);
    Serial.print("BPM : ");
    Serial.println(bpm);
    i++;
    incbpm+=bpm;
    if(i==5){
      avg = incbpm/5;
      ps.pause();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("AVG BPM : ");
      lcd.print(avg);
  }
  if(avg < 80 && avg > 0) music.play(1);
  else if (avg < 101 && avg > 79) music.play(2);
  else if (avg < 121 && avg > 99) music.play(3);
  else if (avg < 141 && avg > 119) music.play(4);
  else if (avg < 161 && avg > 139) music.play(5);
  else if (avg < 181 && avg > 159) music.play(6);
  else if (avg > 180) music.play(7);
  }
  sw_state = digitalRead(13);
  if(sw_state != sw_pre_state) {
    if(sw_state == HIGH) {
      music.pause();
      i = 0;
      avg = 0;
      incbpm = 0;
      reset();
    }
  }
  sw_state = sw_pre_state;
}

