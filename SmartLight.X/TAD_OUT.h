#ifndef TAD_OUT_H
#define	TAD_OUT_H

#define NUM_LEDS 6

void OUT_Init(void);
void OUT_actualizeIntensities(char* intensities);
void OUT_start(void);
void OUT_stop(void);
void OUT_Motor(void);

#endif