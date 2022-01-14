#include <DueTimer.h>

#include <SPI.h>
#include <LoRa.h>
#include "LPF.c"


#define BUFFER_SIZE 220
#define USE_FILTER true
#define PRELOAD_TIMER 10000
#define PLAYBACK_TIMER 250

volatile byte buff[255 * BUFFER_SIZE];
volatile unsigned int pp; // EXCLUSIVE
volatile unsigned int rp; // INCLUSIVE

volatile bool started = false;

volatile int _yold = 0;
volatile LPF LPF1;

void setup() {
  //Serial.begin(9600);
  //while (!Serial);

  //Serial.println("LoRa Receiver");
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  if (!LoRa.begin(433E6)) {
    //Serial.println("Starting LoRa failed!");
    while (1);
  } else {
    LoRa.setTxPower(20);
    LoRa.setSignalBandwidth(500E3);
    LoRa.setSpreadingFactor(6);
    LoRa.enableCrc();
    LoRa.onReceive(onReceive);
    LoRa.receive(255);
  }

  analogWriteResolution(10);

  for (int i = 0; i < 255 * BUFFER_SIZE; i++) {
    buff[i] = 0;
  }

  pp = 0;
  rp = 255 * BUFFER_SIZE - 1;

  LPF_init((LPF*)&LPF1);

  Timer3.attachInterrupt(playback).start(PLAYBACK_TIMER);
  // Timer5.attachInterrupt(resetPB);

  //Serial.println("Started.");
}

void playback() {

  if (((rp + 1) % (255 * BUFFER_SIZE)) != pp) {
    if (USE_FILTER) {
      LPF_put((LPF*)&LPF1, (int)(buff[pp] * 2.5 + 77));
      _yold = LPF_get((LPF*)&LPF1);
      // _yold = filter(_yold, (int)(buff[pp] * 2.5 + 77));
    } else {
      _yold = (int)(buff[pp] * 2.5 + 77);
    }
    pp = ((pp + 1) % (255 * BUFFER_SIZE));
    analogWrite(DAC0, (int)(_yold));
  } else {
    //analogWrite(DAC0, 0);
    //Timer4.stop();
    //Timer5.start(PRELOAD_TIMER);
  }
}


void resetPB() {
  //analogWrite(DAC0, 0);
  //Timer5.stop();
  //Timer4.start(PLAYBACK_TIMER);
}

int filter(int yold, int x) {
  return (int)(exp(-1 * 3000.0 * 0.00014 ) * yold + (1 - exp(-1 * 3000.0 * 0.00014)) * x);
}

void onReceive(int packetSize) {
  if (packetSize) {
    while (LoRa.available() > 0) {
      started = true;
      rp = ((rp + 1) % (255 * BUFFER_SIZE));
      buff[rp] = LoRa.read();
    }
  }
}

void loop() {
}
