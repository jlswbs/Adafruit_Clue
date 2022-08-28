// Broken ant cellular automata //

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

#define WIDTH   240
#define HEIGHT  240
#define SCR     (WIDTH * HEIGHT)

#define ITER    1000

  int x, y;
  int antsdir;

void rndseed(){

  memset(framebuffer, 0, 2*SCR);

  x = rand()%WIDTH;
  y = rand()%HEIGHT;
  antsdir = rand()%4;

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
    
    if (framebuffer[x+ARCADA_TFT_WIDTH*y] > BLACK ){ antsdir = antsdir - 1; framebuffer[x+ARCADA_TFT_WIDTH*y] = BLACK; }
    else { antsdir = antsdir + 1; 
    
    if (antsdir == 0) framebuffer[x+ARCADA_TFT_WIDTH*y] = WHITE; 
    if (antsdir == 1) framebuffer[x+ARCADA_TFT_WIDTH*y] = RED;
    if (antsdir == 2) framebuffer[x+ARCADA_TFT_WIDTH*y] = GREEN; 
    if (antsdir == 3) framebuffer[x+ARCADA_TFT_WIDTH*y] = BLUE;
    }

    if (antsdir > 3) antsdir = 0;   
    if (antsdir < 0) antsdir = 3;   
    
    if (antsdir == 0) x = x - 1;
    if (antsdir == 1) y = y + 1;
    if (antsdir == 2) x = x + 1;
    if (antsdir == 3) y = y - 1;

    if (x > WIDTH-1) x = 0;
    if (x < 0) x = WIDTH-1;
    if (y > HEIGHT-1) y = 0;
    if (y < 0) y = HEIGHT-1;
    
  }
    
  arcada.blitFrameBuffer(0, 0, true, false);
 
}
