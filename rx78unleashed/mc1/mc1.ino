/**
TORSO_BOARD MC1 Attiny85
**/

#define PIN_SHOULDERS 1
#define PIN_VERNIERS 0
#define PIN_SYNC 2

#define TH_F1 50
#define TH_F0 150


const byte gamma[] PROGMEM = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
  5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7,
  7, 7, 7, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10,
  10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15,
  15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21,
  22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 28, 28, 29, 30, 30,
  31, 32, 32, 33, 34, 35, 35, 36, 37, 38, 39, 40, 40, 41, 42, 43,
  44, 45, 46, 47, 48, 49, 51, 52, 53, 54, 55, 56, 58, 59, 60, 62,
  63, 64, 66, 67, 69, 70, 72, 73, 75, 77, 78, 80, 82, 84, 86, 88,
  90, 91, 94, 96, 98, 100, 102, 104, 107, 109, 111, 114, 116, 119, 122, 124,
  127, 130, 133, 136, 139, 142, 145, 148, 151, 155, 158, 161, 165, 169, 172, 176,
  180, 184, 188, 192, 196, 201, 205, 210, 214, 219, 224, 229, 234, 239, 244, 250, 255
};

struct __attribute__((packed)) Led {
  byte pin;
  unsigned long t0 = 0;
  byte stage = 0;
};

byte mode = 0;
byte modeCount = 0;

Led verniers;
Led shoulders;

void setLed(Led* led, byte pwm_value) {
  analogWrite(led->pin, pgm_read_byte(&gamma[pwm_value]));
}

Led initLed(Led* led, byte pin) {
  led->pin = pin;
  led->stage = 0;
  led->t0 = 0;
  pinMode(pin, OUTPUT);
  analogWrite(pin, 0);
}

void next(Led* led) {
  led->stage++;
  led->t0 = millis();
}

void reset(Led* led) {
  led->stage = 0;
  led->t0 = millis();
  setLed(led, 0);
}

void wait(Led* led, unsigned long t, byte v) {
  unsigned long d = millis() - led->t0;
  if (d > t) {
    next(led);
    setLed(led, v);
  }
}

byte flicker(Led* led, byte i) {
  byte n = millis() % 42 > 36 ? i : n / 4 * 3;
  analogWrite(led->pin, pgm_read_byte(&gamma[n]));
}

void setup() {
  initLed(&shoulders, PIN_SHOULDERS);
  initLed(&verniers, PIN_VERNIERS);
  pinMode(PIN_SYNC, INPUT_PULLUP);
  while (digitalRead(PIN_SYNC) == 1) {
    delay(1);
  }
}

void runVerniers() {
  unsigned long d = millis() - verniers.t0;
  byte i = 0;
  switch (verniers.stage) {
    case 0:
      next(&verniers);
      break;
    case 1:  //off
      if (d > 3000 + 4500)
        next(&verniers);
      break;
    case 2:  //fade in
      i = map(d, 0, 3000, 0, 255);
      flicker(&verniers, i);
      if (d > 3000)
        next(&verniers);
      break;
    case 3:  //on
      if (d > 8500) next(&verniers);
      flicker(&verniers, 255);
      break;
    case 4:  //fade out
      i = map(d, 0, 2000, 255, 0);
      flicker(&verniers, i);
      if (d > 2000)
        next(&verniers);
      break;
  }
}

void runShoulders() {
  byte i = 0;
  unsigned long d = millis() - shoulders.t0;
  switch (shoulders.stage) {
    case 0:
      next(&shoulders);
      break;
    case 1:  // off
      wait(&shoulders, 3000, 255);
      break;
    case 2:  // blink1
      wait(&shoulders, TH_F1, 0);
      break;
    case 3:  // off
      wait(&shoulders, TH_F0, 255);
      break;
    case 4:  // blink2
      wait(&shoulders, TH_F1, 0);
      break;
    case 5:  // off
      wait(&shoulders, TH_F0, 255);
      break;
    case 6:  // blink3
      wait(&shoulders, TH_F1, 0);
      break;
    case 7:  // off
      wait(&shoulders, 1000, 0);
      break;
    case 8:  //fade in
      i = map(d, 0, 500, 0, 250);
      setLed(&shoulders, i);
      wait(&shoulders, 500, 250);
      break;
    case 9:  // on
      wait(&shoulders, 15050, 0);
      break;
    case 10:  //fade OUT
      i = map(d, 500, 0, 0, 255);
      setLed(&shoulders, i);
      if (d > 500)
        next(&shoulders);
      break;
    case 11:
      wait(&shoulders, 500, 0);
      break;
  }
}

void loop() {
  runVerniers();
  runShoulders();
  if (digitalRead(PIN_SYNC) == 0) {
    while (digitalRead(PIN_SYNC) == 0) delay(1);
    reset(&shoulders);
    reset(&verniers);
  }
}
