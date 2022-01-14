#include <SPI.h>
#include <LoRa.h>


#include <ESP8266WiFi.h>

#define ss 15
#define rst 16
#define dio0 2

#define BUFFER_SIZE 100

volatile byte buff[255 * BUFFER_SIZE];
volatile byte buff2[255 * BUFFER_SIZE];
volatile int bi = 0;
volatile int bi2 = 0;
volatile bool firstBuff = true;

//unsigned long int o = 0;

//=======================================================================
void ICACHE_RAM_ATTR onTimerISR(){
  int readed = analogRead(A0);
  byte aud = (byte)((readed - 77) / 2.5);
  if (firstBuff) {
    buff[bi++] = aud;
    bi = min((int)bi, 255 * BUFFER_SIZE);
  } else {
    buff2[bi2++] = aud;
    bi2 = min((int)bi2, 255 * BUFFER_SIZE);
  }
  timer1_write(700);
}
//=======================================================================

void copy(byte* src, byte* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
 
  
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    //Serial.println("Starting LoRa failed!");
    while (1) {
      delay(1);
    }
  } else {
    LoRa.setTxPower(20);
    LoRa.setSignalBandwidth(500E3);
    LoRa.setSpreadingFactor(6);
    LoRa.enableCrc();
    timer1_attachInterrupt(onTimerISR);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
    timer1_write(700); //0.0002496 s
  }

  
  
  //Serial.println("Started.");
  //o = micros();
}

// the loop routine runs over and over again forever:
void loop() {
  
  if ((firstBuff ? bi : bi2) >= (BUFFER_SIZE * 255)) {
  
    firstBuff = !firstBuff;
    //o = micros() - o;
    // 
    //Serial.println("REC: " + String(o) + " B" + (!firstBuff ? "1" : "2"));
    //o = micros();
    //digitalWrite(2, HIGH);
    //byte _buff[255];
    //copy((byte *)buff, _buff, 254);
    //noInterrupts();
    for (int i = 0; i < BUFFER_SIZE; i++) {

      LoRa.beginPacket(true);
      if (!firstBuff)
        LoRa.write(((byte *)buff) + (255 * i), 255);
      else
        LoRa.write(((byte *)buff2) + (255 * i), 255);
      LoRa.endPacket();
      //Serial.println("Bi1 : " + String(bi));
      //Serial.println("Bi2 : " + String(bi2));
    }
    if (!firstBuff)
      bi = 0;
    else
      bi2 = 0;
    //Serial.println("Bi1 : " + String(bi));
    //Serial.println("Bi2 : " + String(bi2));
    //digitalWrite(2, LOW);
    //Serial.println("TRANSMIT: " + String(micros() - o));
    // timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
    // timer1_write(625); //0.0002496 s
    //o = micros();
    //interrupts();
  }

  
}
