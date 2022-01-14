//#include "LPF.h"

/*

FIR filter designed with
http://t-filter.appspot.com

sampling frequency: 7143 Hz

fixed point precision: 16 bits

* 0 Hz - 2000 Hz
  gain = 1
  desired ripple = 1 dB
  actual ripple = n/a

* 2800 Hz - 3571.5 Hz
  gain = 0
  desired attenuation = -80 dB
  actual attenuation = n/a

*/



#define FILTER_TAP_NUM 67

static int filter_taps[FILTER_TAP_NUM] = {
  750,
  721,
  -373,
  122,
  62,
  -189,
  257,
  -260,
  194,
  -68,
  -91,
  246,
  -351,
  366,
  -272,
  78,
  174,
  -417,
  578,
  -591,
  422,
  -84,
  -356,
  789,
  -1085,
  1114,
  -792,
  87,
  946,
  -2184,
  3451,
  -4546,
  5288,
  27218,
  5288,
  -4546,
  3451,
  -2184,
  946,
  87,
  -792,
  1114,
  -1085,
  789,
  -356,
  -84,
  422,
  -591,
  578,
  -417,
  174,
  78,
  -272,
  366,
  -351,
  246,
  -91,
  -68,
  194,
  -260,
  257,
  -189,
  62,
  122,
  -373,
  721,
  750
};




typedef struct {
 int history[FILTER_TAP_NUM];
 unsigned int last_index;
} LPF;


void LPF_init(LPF* f) {
 int i;
 for(i = 0; i < FILTER_TAP_NUM; ++i)
   f->history[i] = 0;
 f->last_index = 0;
}

void LPF_put(LPF* f, int input) {
 f->history[f->last_index++] = input;
 if(f->last_index == FILTER_TAP_NUM)
   f->last_index = 0;
}

int LPF_get(LPF* f) {
 long long acc = 0;
 int index = f->last_index, i;
 for(i = 0; i < FILTER_TAP_NUM; ++i) {
   index = index != 0 ? index-1 : FILTER_TAP_NUM-1;
   acc += (long long)f->history[index] * filter_taps[i];
 };
 return acc >> 16;
}
