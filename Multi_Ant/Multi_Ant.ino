// Multi Langton's ant cellular automata //

#include "Adafruit_Arcada.h"

#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

Adafruit_Arcada arcada;

uint16_t *framebuffer;

#define WIDTH  240
#define HEIGHT 240
#define SCR (WIDTH * HEIGHT)

#define NUMANTS 6
#define ITER    100

  uint16_t coll[NUMANTS];
  int x[NUMANTS];
  int y[NUMANTS];
  int antsdir[NUMANTS];


void rndseed(){

  memset(framebuffer, 0, 2*SCR);

  for(int i = 0; i < NUMANTS; i++){
  
    x[i] = rand()%WIDTH;
    y[i] = rand()%HEIGHT;
    antsdir[i] = rand()%4;
    coll[i] = rand();
    
  }

}


void setup(void) {

  unsigned short hwrng = 0;
  NRF_RNG->TASKS_START = 1;
  NRF_RNG->EVENTS_VALRDY = 0;
  while(!NRF_RNG->EVENTS_VALRDY);
  hwrng = (NRF_RNG->VALUE & 0xFF);
  NRF_RNG->EVENTS_VALRDY = 0;
  while(!NRF_RNG->EVENTS_VALRDY);
  hwrng |= ((NRF_RNG->VALUE & 0xFF) << 8);
  NRF_RNG->TASKS_STOP = 1;
 
  srand(hwrng);

  if (!arcada.arcadaBegin()) {while (1);}
  arcada.displayBegin();
  arcada.setBacklight(255);
  if (! arcada.createFrameBuffer(ARCADA_TFT_WIDTH, ARCADA_TFT_HEIGHT)) {while (1);}
  framebuffer = arcada.getFrameBuffer();

  rndseed();
 
}

void loop() {

  if (arcada.readButtons() & ARCADA_BUTTONMASK_A) rndseed();

  for(int k = 0; k < ITER; k++){
  
    for(int i = 0; i < NUMANTS; i++){
    
      if (framebuffer[x[i]+ARCADA_TFT_WIDTH*y[i]] > BLACK){ antsdir[i] = antsdir[i] - 1; framebuffer[x[i]+ARCADA_TFT_WIDTH*y[i]] = BLACK; }
      else { antsdir[i] = antsdir[i] + 1; framebuffer[x[i]+ARCADA_TFT_WIDTH*y[i]] = coll[i]; }

      if (antsdir[i] > 3) antsdir[i] = 0;   
      if (antsdir[i] < 0) antsdir[i] = 3;   
    
      if (antsdir[i] == 0) x[i] = x[i] - 1;
      if (antsdir[i] == 1) y[i] = y[i] + 1;
      if (antsdir[i] == 2) x[i] = x[i] + 1;
      if (antsdir[i] == 3) y[i] = y[i] - 1;
    
      if (x[i] > WIDTH-1) x[i] = 0;
      if (x[i] < 0) x[i] = WIDTH-1;
      if (y[i] > HEIGHT-1) y[i] = 0;
      if (y[i] < 0) y[i] = HEIGHT-1;
    
    }
    
  }
    
  arcada.blitFrameBuffer(0, 0, true, false);
 
}
