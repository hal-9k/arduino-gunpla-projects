/**
BASE_BOARD MC3 nano
**/
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

#define PIN_DEMO A2
#define PIN_MUTE A3
#define PIN_PLAYER_RX A0
#define PIN_PLAYER_TX A1
#define PIN_SYNC A4

#define MODE_ANNOYING 0
#define MODE_DEMO 1
#define MODE_MUTE 2

struct __attribute__((packed)) Player {
  unsigned long t0 = 0;
  byte stage = 0;
};

byte mode = 0;
byte modeCount = 0;

Player sfx;
DFRobotDFPlayerMini player;
SoftwareSerial ss(PIN_PLAYER_RX, PIN_PLAYER_TX);

void next(Player* p) {
  p->stage++;
  p->t0 = millis();
}

void reset(Player* p) {
  p->stage = 0;
  p->t0 = millis();
}

void wait(Player* p, unsigned long t) {
  unsigned long d = millis() - p->t0;
  if (d > t) {
    next(p);
  }
}

void setup() {

  ss.begin(9600);
  if (!player.begin(ss, /*isACK = */ true, /*doReset = */ true)) {
    pinMode(PIN_SYNC, OUTPUT);
    while (true) {
      delay(0);
    }
  }
  player.volume(15);  //Set volume value. From 0 to 30
  pinMode(PIN_MUTE, INPUT_PULLUP);
  pinMode(PIN_DEMO, INPUT_PULLUP);
  pinMode(PIN_SYNC, OUTPUT);
  if (digitalRead(PIN_MUTE) == 0) mode = 2;
  if (digitalRead(PIN_DEMO) == 0) mode = 1;
}


void runPlayer() {
  unsigned long d = millis() - sfx.t0;
  switch (sfx.stage) {
    case 0:
      //SYNC->GO!
      digitalWrite(PIN_SYNC, 0);
      delay(20);
      digitalWrite(PIN_SYNC, 1);
      next(&sfx);
      break;
    case 1:
      wait(&sfx, 2750);
      break;
    case 2:
      switch (mode) {
        case MODE_MUTE:
          break;
        case MODE_DEMO:
          modeCount++;
          if (modeCount < 15) break;  //5mins~ish wait between sounds
          modeCount = 0;
        case MODE_ANNOYING:
          player.play(1);
      }
      next(&sfx);
      break;
    case 3:
      wait(&sfx, 20000);
      break;
    case 4:
      reset(&sfx);
      break;
  }
}

void loop() {
  runPlayer();
}
