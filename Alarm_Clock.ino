#include <Wire.h>
#include <Keypad.h>
#include "U8glib.h"
#include <RtcDS3231.h>

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE);//skärm initiallisering
RtcDS3231<TwoWire> rtcModule(Wire); //rtc initiallisering

byte hours;
byte minutes;
byte seconds;

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {//skapa en map för keypaden
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {8, 7, 6, 5};
byte colsPins[COLS] = {4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colsPins, ROWS, COLS);
char key = keypad.getKey(); // säger att 'key' är variabeln för knapparna


const int piezo = 10;
const int button = 9;

int i = 1; //counter för vilken sida man är på
int j = 0; //variabel för att sätta på och av timern
int k = 0; //counter för att kolla om man ska ha på countern
int p = 0; //variabel för att ställa in alarm klockan

int secondBegin = 0; //variabel för att räkna tiden i countern
int minuteBegin = 0; //variabel för att räkna tiden i countern
int hourBegin = 0; //variabel för att räkna tiden i countern

int dif_second = 0; //skillnaden i tid som visas i countern
int dif_minute = 0;//skillnaden i tid som visas i countern
int dif_hour = 0;//skillnaden i tid som visas i countern

int timer_second = 0; //timern som visas på skärmen
int timer_minute = 0; //timern som visas på skärmen
int timer_hour = 0; //timern som visas på skärmen

int timer_temp_second = 0; //variablen som gör att timern kan räkna ned
int timer_temp_minute = 0; //variablen som gör att timern kan räkna ned
int timer_temp_hour = 0; //variablen som gör att timern kan räkna ned

int timer_button_second = 0; //variablen som gör att timern kan räkna ned
int timer_button_minute = 0; //variablen som gör att timern kan räkna ned
int timer_button_hour = 0; //variablen som gör att timern kan räkna ned

int timer_temp = 0;
int timer_temp_m = 0;

int alarm_second = 0; //tiden som visas på skärmen i alarm
int alarm_minute = 0; //tiden som visas på skärmen i alarm
int alarm_hour = 0; //tiden som visas på skärmen i alarm

void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  pinMode(piezo, OUTPUT);
  pinMode(button, INPUT);
  int buttonPressed = digitalRead(button);
  rtcModule.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  rtcModule.SetDateTime(compiled);
}

void drawAlarm() {
  oled.setFont(u8g_font_helvB08);
  RtcDateTime now = rtcModule.GetDateTime();
  char key = keypad.getKey();
  hours = now.Hour();
  minutes = now.Minute();
  seconds = now.Second();
  oled.setPrintPos(75, 11);
  oled.print(String(hours) + ": " + String(minutes) + ": " + String(seconds));
  oled.setFont(u8g_font_helvB10);
  oled.drawStr(5, 12, "Alarm");
  oled.setFont(u8g_font_helvB08);
  oled.drawStr(6, 28, "Set The Clock");

  if (digitalRead(button) == HIGH) {
    p += 1;
    if (p > 5) {
      p = 0;
    }
    delay (300);
  }

  if (p == 0){
    if (key == '#'){
      i  += 1;
    }
    if (key == '*') {
      i -= 1;
    }
  }

  if (p == 1) {
    if (key != NO_KEY) {
      if (key == '*'|| key == '#'){
        alarm_hour = 0;
      }
      
      if (alarm_hour > 0){
        int i_key = key - '0';
        alarm_hour = alarm_hour*10 + i_key;
      }
      else{
        int i_key = key - '0';
        alarm_hour = i_key;
      }
    }
    if (alarm_hour >= 24){
      alarm_hour = 23;
    }
    oled.drawStr(6, 60, "Hour");
  }

  if (p == 2) {
    if (key != NO_KEY) {
      if (key == '*'|| key == '#'){
        alarm_minute = 0;
      }
      
      if (alarm_minute > 0){
        int i_key = key - '0';
        alarm_minute = alarm_minute*10 + i_key;
      }
      else{
        int i_key = key - '0';
        alarm_minute = i_key;
      }
    }
    if (alarm_minute > 60){
      alarm_minute = 59;
    }
    oled.drawStr(6, 60, "Minute");
  }

  if (p == 3) {
    if (key != NO_KEY) {
      if (key == '*'|| key == '#'){
        alarm_second = 0;
      }
      
      if (alarm_second > 0){
        int i_key = key - '0';
        alarm_second = alarm_second*10 + i_key;
      }
      else{
        int i_key = key - '0';
        alarm_second = i_key;
      }
      if (alarm_second > 60){
        alarm_second = 60; 
      }
    }
    if (alarm_second > 60){
      alarm_second = 59;
    }
    oled.drawStr(6, 60, "Second");
  }

  if (p == 4){
    if (now.Second() == alarm_second){
      if (now.Minute() == alarm_minute){
        if (now.Hour() == alarm_hour){
          alarm_hour = 0;
          alarm_minute = 0;
          alarm_second = 0;
          p = 0;    
          tone(piezo, 1976);
          delay (200);
          tone(piezo, 2349);
          delay (200);    
          tone(piezo, 3951);
          delay (200);    
          tone(piezo, 3520);
          delay (300);    
          tone(piezo, 2960);
          delay (300);    
          noTone(piezo);
        }
      }
    }
    oled.drawStr(6, 60, "Time Set");
    
    if (key == '#'){
      i  += 1;
    }
    if (key == '*') {
      i -= 1;
    }
  }

  if (p == 5){
    alarm_hour = 0;
    alarm_minute = 0;
    alarm_second = 0;
    p = 0; 

    if (key == '#'){
      i  += 1;
    }
    if (key == '*') {
      i -= 1;
    }
  }

  if (alarm_hour < 0){
    alarm_hour = 0;
  }
  if (alarm_minute < 0){
    alarm_minute = 0;
  }
  if (alarm_second < 0){
    alarm_second = 0;
  }
  
  oled.setPrintPos(6, 45);
  oled.setFont(u8g_font_helvB10);
  oled.print(String(alarm_hour) + ": " + String(alarm_minute) + ": " + String(alarm_second));
}

void drawTimer() {
  oled.setFont(u8g_font_helvB08);
  RtcDateTime now = rtcModule.GetDateTime();
  char key = keypad.getKey();
  hours = now.Hour();
  minutes = now.Minute();
  seconds = now.Second();
  oled.setPrintPos(75, 11);
  oled.print(String(hours) + ": " + String(minutes) + ": " + String(seconds));
  oled.setFont(u8g_font_helvB10);
  oled.drawStr(5, 12, "Timer");
  oled.setFont(u8g_font_helvB08);
  oled.drawStr(6, 28, "Set Time");


  if (digitalRead(button) == HIGH) {
    j += 1;
    if (j > 4) {
      j = 0;
    }
    delay (300);
  }

  if (j == 0){
    if (key == '#'){
      i  += 1;
    }
    if (key == '*') {
      i -= 1;
    }
  }

  if (j == 1) {
    if (key != NO_KEY) {
      if (key == '*'|| key == '#'){
        timer_hour = 0;
      }
      
      if (timer_hour > 0){
        int i_key = key - '0';
        timer_hour = timer_hour*10 + i_key;
      }
      else{
        int i_key = key - '0';
        timer_hour = i_key;
      }
    }
    oled.drawStr(6, 60, "Hour");
  }

  if (j == 2) {
    if (key != NO_KEY) {
      if (key == '*'|| key == '#'){
        timer_minute = 0;
      }
      
      if (timer_minute > 0){
        int i_key = key - '0';
        timer_minute = timer_minute*10 + i_key;
      }
      else{
        int i_key = key - '0';
        timer_minute = i_key;
      }
    }
    oled.drawStr(6, 60, "Minute");
  }

  if (j == 3) {
    if (key != NO_KEY) {
      if (key == '*'|| key == '#'){
        timer_second = 0;
      }
      
      if (timer_second > 0){
        int i_key = key - '0';
        timer_second = timer_second*10 + i_key;
      }
      else{
        int i_key = key - '0';
        timer_second = i_key;
      }
      if (timer_second > 60){
        timer_second = 60; 
      }
    }
    timer_button_second = now.Second();
    timer_button_minute = now.Minute(); 
    timer_button_hour = now.Hour();
    timer_temp = now.Second() - 60;
    timer_temp_m = now.Minute() + 1;
    
    timer_temp_second = timer_second;
    timer_temp_minute = timer_minute;
    timer_temp_hour = timer_hour;

    oled.drawStr(6, 60, "Second");
  }
  
  if (timer_hour < 0){
    timer_hour = 0;
  }
  if (timer_minute < 0){
    timer_minute = 0;
  }
  if (timer_second < 0){
    timer_second = 0;
  }
  
  if (j == 4) {
    timer_second = timer_temp_second;
    timer_minute = timer_temp_minute;
    timer_hour = timer_temp_hour;
    
    if (now.Second() == 0){
      timer_button_second = timer_temp;
      timer_button_minute = timer_temp_m;
    }
    
    timer_second = timer_second - now.Second() + timer_button_second;
    timer_minute = timer_minute - now.Minute() + timer_button_minute;
    timer_hour = timer_hour - now.Hour() + timer_button_hour; 
    
    if (key == '#'){
      i  += 1;
    }
    if (key == '*') {
      i -= 1;
    }
  }

  if (timer_second == -1) { //if sats för att inte få negativ tid
    timer_minute -= 1;
    timer_second += 60;
  }
  
  if (timer_minute == -1) { //if sats för att inte få negativ tid
    timer_hour -= 1;
    timer_minute += 60;
  }

  if (timer_hour == -1){ //if sats för att kolla när tiden tatt slut
    timer_hour = 0;
    timer_minute = 0;
    timer_second = 0;
    j = 0;    
    tone(piezo, 1976);
    delay (200);
    tone(piezo, 2349);
    delay (200);    
    tone(piezo, 3951);
    delay (200);    
    tone(piezo, 3520);
    delay (300);    
    tone(piezo, 2960);
    delay (300);    
    noTone(piezo);
  }
  
  oled.setPrintPos(6, 45);
  oled.setFont(u8g_font_helvB10);
  oled.print(String(timer_hour) + ": " + String(timer_minute) + ": " + String(timer_second));
}

void drawCounter() {

  oled.setFont(u8g_font_helvB08);
  RtcDateTime now = rtcModule.GetDateTime();
  hours = now.Hour();
  minutes = now.Minute();
  seconds = now.Second();
  oled.setPrintPos(75, 11);
  oled.print(String(hours) + ": " + String(minutes) + ": " + String(seconds));
  oled.setFont(u8g_font_helvB10);
  oled.drawStr(5, 12, "Counter");
  oled.setFont(u8g_font_helvB08);
  oled.drawStr(6, 28, "Start");

  if (digitalRead(button) == HIGH) {
    k += 1;
    if (k > 1) {
      k = 0;
    }
    Serial.println(k);
    delay (400);
  }

  if (k == 0) {
    secondBegin = now.Second();
    minuteBegin = now.Minute();
    hourBegin = now.Hour();

    Serial.println(secondBegin);

    dif_second = now.Second() - secondBegin;
    dif_minute = now.Minute() - minuteBegin;
    dif_hour = now.Hour() - hourBegin;
  }

  if (k == 1) {
    dif_second = now.Second() - secondBegin;
    dif_minute = now.Minute() - minuteBegin;
    dif_hour = now.Hour() - hourBegin;
  }

  if (dif_second < 0) { //if sats för att inte få negativ tid
    dif_minute -= 1;
    dif_second += 60;
  }

  if (dif_minute < 0) { //if sats för att inte få negativ tid
    dif_hour -= 1;
    dif_minute += 60;
  }

  oled.setPrintPos(6, 45);
  oled.setFont(u8g_font_helvB10);
  oled.print(String(dif_hour) + ": " + String(dif_minute) + ": " + String(dif_second));

}

void loop() {
  RtcDateTime now = rtcModule.GetDateTime();
  char key = keypad.getKey(); // säger att 'key' är variabeln för knapparna
  if (key == '#') {// om '#' är tryck
    i += 1;
  }
  if (key == '*') {// om '*' är tryck
    i -= 1;
  }

  if (i < 1) { //göra att om man går förbi första sidan hoppar man till slutet
    i = 3;
  }

  if (i > 3) {//göra att om man går förbi tre kommer man till första sidan
    i = 1;
  }

  if (i == 1) { //gå till alarm
    oled.firstPage();
    do {
      drawAlarm();
    } while (oled.nextPage());
  }

  if (i == 2) { //gå till timern
    oled.firstPage();
    do {
      drawTimer();
    } while (oled.nextPage());
  }

  if (i == 3) { //gå till counter
    oled.firstPage();
    do {
      drawCounter();
    } while (oled.nextPage());
  }
}
