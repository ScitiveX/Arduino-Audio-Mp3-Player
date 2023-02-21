// Code by : ScitiveX
// Arduino Audio Mp3 Player using 1.8 inch tft color display
//
//    ___              _      _        _                    __  __
//   / __|    __      (_)    | |_     (_)    __ __    ___   \ \/ /
//   \__ \   / _|     | |    |  _|    | |    \ V /   / -_)   >  <
//   |___/   \__|_   _|_|_   _\__|   _|_|_   _\_/_   \___|  /_/\_\  
// _|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|
// "`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'


// Libraries

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Arduino_ST7735_Fast.h>

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

//Images
#include "waveanimation.h"
#include "buttons.h"

uint16_t* wave1[] = {ani1_0,ani1_1};
const int numWave1 = sizeof(wave1) / sizeof(wave1[0]);

SoftwareSerial mySoftwareSerial(2, 3); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

const byte left = A2;
const byte right = A0;
const byte ok = A1;
const byte vol_pin = A3;

// Booleans for input states
bool left_state = LOW;
bool right_state = LOW;
bool ok_state = LOW;

int max_mp3, vol = 15;

int playpauseb_x = 53, gap = 7;
// Display width & height
#define SCR_WD   128
#define SCR_HT   160

// Display SPI pins
#define TFT_CS 10
#define TFT_RST  8
#define TFT_DC 9

Arduino_ST7735 lcd = Arduino_ST7735(TFT_DC, TFT_RST, TFT_CS);

int song_n = 1;
bool ppflag = 1;

int folder = 15; // folder name 
byte wavex =25, wavey=60;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.fillScreen(RGBto565(0, 0, 0));
  lcd.setRotation(2);

  mySoftwareSerial.begin(9600);
  sdmsg("Loading...");
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    sdmsg("Error");

    while (true) {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  sdmsg("System connected.");
  startscreen(2000);

  max_mp3 = myDFPlayer.readFileCountsInFolder(folder); 

  Serial.println("file in folder " +(String)folder + " : " + (String)max_mp3);
  myDFPlayer.volume(vol);  //Set volume value. From 0 to 30
  delay(500);


  pinMode(left, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);
  pinMode(ok, INPUT_PULLUP);
  pinMode(vol_pin, INPUT);
  myDFPlayer.playFolder(folder, song_n);
  delay(500);
  lcd.drawLine(0, 132, 128, 132, WHITE);
  lcd.drawImageF(playpauseb_x, 120, playbw, playbh, playb);
  lcd.drawImageF(playpauseb_x - gap - l_r_b_wh, 122, l_r_b_wh, l_r_b_wh, leftsmallb);
  lcd.drawImageF(playpauseb_x + playbw + gap, 122, l_r_b_wh, l_r_b_wh, rightsmallb);
  delay(10);
  lcd.drawRect(wavex-1,wavey-1,ani1w+2,ani1h+2,WHITE);

}




void loop() {

  if (ppflag == 1) {
    playscreen();
  }
  else if (ppflag == 0 || myDFPlayer.readState()==512) {
    pausescreen();
    delay(40);
  }

  checkbut();
  print_info();
  sound_screen();

}

// check which button is pressing
void checkbut()
{
  if (digitalRead(left) == LOW) {
    Serial.println("left");
    song_n--;
    if (song_n < 1) {
      song_n = max_mp3;
    }
    ppf();
    lcd.drawImageF(playpauseb_x - gap - l_r_b_wh, 122, l_r_b_wh, l_r_b_wh, leftbigb);
    delay(100);
    lcd.drawImageF(playpauseb_x - gap - l_r_b_wh, 122, l_r_b_wh, l_r_b_wh, leftsmallb);
    delay(60);
  }


  if (digitalRead(right) == LOW) {
    Serial.println("right");
    song_n++;
    if (song_n > max_mp3) {
      song_n = 1;
    }

    ppf();
    lcd.drawImageF(playpauseb_x + playbw + gap, 122, l_r_b_wh, l_r_b_wh, rightbigb);
    delay(100);
    lcd.drawImageF(playpauseb_x + playbw + gap, 122, l_r_b_wh, l_r_b_wh, rightsmallb);
    delay(60);

  }

  if (digitalRead(ok) == LOW) {

    ppflag = !ppflag;
    if (ppflag == 0) {
      myDFPlayer.pause();
      pausescreen();
    } else if (ppflag == 1) {
      myDFPlayer.start();

      Serial.println("play");
      lcd.drawImageF(playpauseb_x, 120, playbw, playbh, playb);
      delay(60);
    }ppf();

  }

}

// update to pause button
void pausescreen() {
  ppflag = 0;
  lcd.drawImageF(playpauseb_x, 120, playbw, playbh, pauseb);
  delay(40);
  Serial.println("pause");
}

// show wave animation
void playscreen() {
  lcd.drawImageF(playpauseb_x, 120, playbw, playbh, playb);
  ppflag = 1;
  for (int i = 0; i < numWave1; i++) {
    lcd.drawImageF(wavex, wavey, ani1w, ani1h, wave1[i]);
    delay(40);
  }
}

void ppf() {
  if (ppflag == 1) {
    myDFPlayer.playFolder(folder, song_n);
    Serial.println("PLaying");
    playscreen();
  } else if (ppflag == 0) {
    pausescreen();
    Serial.println("Paused");
  }

}


void print_info() {
  lcd.setTextColor(WHITE);
  lcd.setTextSize(1);
  lcd.fillRect(0, 0, 160, 23, (RGBto565(28, 161, 226)));

  if ( ppflag == 1) {
    lcd.setCursor(16, 7);
    lcd.print(" Playing : ");

  } else if (ppflag == 0) {
    lcd.setCursor(16, 7);
    lcd.print(" Paused : ");
  }
  lcd.print((String)song_n);
  lcd.print(".mp3");
  delay(2);

}

void sdmsg(String s) {

  lcd.fillRect(0, 0, 160, 23, (RGBto565(28, 161, 226)));
  lcd.setCursor(12, 7);
  lcd.setTextColor(WHITE);
  lcd.setTextSize(1);
  lcd.println(s);
  delay(10);
}

void startscreen(int t) {
  //lcd.fillRect(0, 0, 160, 23, (RGBto565(28, 161, 226)));

  lcd.setTextColor(WHITE);
  lcd.setTextSize(3);
  lcd.setCursor(20, 50);
  lcd.println("Audio");
  lcd.setCursor(14, 90);
  lcd.println("Player");
  delay(t);
  lcd.fillRect(0, 23, 128, 160, (RGBto565(0, 0, 0)));
  delay(10);

}

void sound_screen() {
  int vol_val = analogRead(vol_pin);
  vol = map(vol_val, 0, 1023, 0, 30);
  int vwidth = map(vol_val, 0, 1023, 0, SCR_WD + 5); // 2 tol/error pf pot
  myDFPlayer.volume(vol);

  lcd.drawLine(0, 23, SCR_WD, 23, BLACK);

  if (vol <= 10) {
    lcd.drawLine(0, 23, vwidth, 23, WHITE);
  }
  else if (vol > 10 && vol <= 20) {
    lcd.drawLine(0, 23, vwidth, 23, GREEN);
  }
  else if (vol > 20 && vol <= 30) {
    lcd.drawLine(0, 23, vwidth, 23, RED);
  }
  delay(50);
}
