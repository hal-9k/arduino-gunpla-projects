
/**
TORSO_BOARD MC2 Attiny85
**/

#define PIN_SABER 4
#define PIN_EYES 0
#define PIN_CHEST 1
#define PIN_SYNC 2

#define SABER_CYCLE 15

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

struct __attribute__((packed)) Saber {
  Led* led;
  byte cycle = 0;
  byte duty = 0;
  byte isFull = 0;
  unsigned long t1;
};

byte mode = 0;
byte modeCount = 0;

Led eyes;
Led chest;
Led ledSaber;
Saber saber;

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

void resetSaber(Saber* s, byte hasFull) {
  s->cycle = SABER_CYCLE;
  s->isFull = random(10) > 8 ? hasFull : 0;
  s->duty = random(SABER_CYCLE);
  s->t1 = millis();
}

byte randomFlicker(Saber* saber, byte i) {
  byte n = millis() % saber->cycle > saber->duty ? i : n / 4 * 3;
  if (saber->isFull) {
    analogWrite(saber->led->pin, pgm_read_byte(&gamma[n]));
  } else {
    flicker(saber->led, n);
  }
  if (millis() - saber->t1 > saber->cycle) resetSaber(saber, 1);
}

void setup() {

  initLed(&eyes, PIN_EYES);
  initLed(&chest, PIN_CHEST);
  initLed(&ledSaber, PIN_SABER);
  saber.led = &ledSaber;
  pinMode(PIN_SYNC, INPUT_PULLUP);
  while (digitalRead(PIN_SYNC) == 1)
    delay(1);
}

void runChest() {
  unsigned long d = millis() - chest.t0;
  byte i = 0;
  switch (chest.stage) {
    case 0:
      next(&chest);
      break;
    case 1:  //off
      if (d > 3000 + 4750)
        next(&chest);
      break;
    case 2:  //fade in
      i = map(d, 0, 750, 255, 128);
      setLed(&chest, i);
      if (d > 750)
        next(&chest);
      break;
    case 3:  //on
      if (d > 11500) next(&chest);
      break;
    case 4:  //fade out
      i = map(d, 0, 1000, 128, 0);
      setLed(&chest, i);
      if (d > 1000)
        next(&chest);
      break;
  }
}

void runEyes() {
  unsigned long d = millis() - eyes.t0;
  byte i = 0;
  switch (eyes.stage) {
    case 0:
      next(&eyes);
      break;
    case 1:  //off
      if (d > 2500 + 3000)
        next(&eyes);
      break;
    case 2:  //fade in
      i = map(d, 0, 1000, 255, 180);
      setLed(&eyes, i);
      if (d > 1000)
        next(&eyes);
      break;
    case 3:  //on
      if (d > 13500) next(&eyes);
      break;
    case 4:  //fade out
      i = map(d, 0, 1000, 255, 0);
      setLed(&eyes, i);
      if (d > 1000)
        next(&eyes);
      break;
  }
}

void runSaber() {

  unsigned long d = millis() - saber.led->t0;
  byte i = 0;
  switch (saber.led->stage) {
    case 0:
      next(saber.led);
      break;
    case 1:  //off
      if (d > 3000 + 6000)
        next(saber.led);
      break;
    case 2:  //fade in
      i = map(d, 0, 500, 0, 255);
      randomFlicker(&saber, i);
      wait(saber.led, 500, 255);
      break;
    case 3:  //on
      wait(saber.led, 9500, 255);
      randomFlicker(&saber, 255);
      break;
    case 4:  //fade out
      i = map(d, 0, 1500, 255, 0);
      randomFlicker(&saber, i);
      wait(saber.led, 1500, 0);
      break;
    case 5:
      wait(saber.led, 500, 0);
      break;
  }
}

void loop() {
  runSaber();
  runChest();
  runEyes();
  if (digitalRead(PIN_SYNC) == 0) {
    while (digitalRead(PIN_SYNC) == 0) delay(1);
    reset(&chest);
    reset(saber.led);
    reset(&eyes);
  }
}
