// Voronoi distribution (cell noise) //

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

#define WIDTH  120
#define HEIGHT 120
#define SCR (ARCADA_TFT_WIDTH * ARCADA_TFT_HEIGHT)

uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }

#define PARTICLES 25

  float mindist = 0;
  bool colour = false;
  int x[PARTICLES];
  int y[PARTICLES];
  int dx[PARTICLES];
  int dy[PARTICLES];

float distance(int x1, int y1, int x2, int y2) { return sqrtf(powf(x2 - x1, 2.0f) + powf(y2 - y1, 2.0f) * 1.0f); } 

void rndrule(){  

  memset(framebuffer, 0, 2*SCR);
  
  for (int i=0; i<PARTICLES; i++) {
    x[i] = rand()%WIDTH;
    y[i] = rand()%HEIGHT;
    dx[i] = rand()%15;
    dy[i] = rand()%15;
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

  rndrule();
 
}

void loop() {

  if (arcada.readButtons() & ARCADA_BUTTONMASK_A) rndrule();
  if (arcada.readButtons() & ARCADA_BUTTONMASK_B) colour = !colour;

  for (int j=0; j<HEIGHT; j++) {  
    for (int i=0; i<WIDTH; i++) {
      
      mindist = WIDTH;
      
      for (int p=0; p<PARTICLES; p++) {
        if (distance(x[p], y[p], i, j) < mindist) mindist = distance(x[p], y[p], i, j);
        if (distance(x[p]+WIDTH, y[p], i, j) < mindist) mindist = distance(x[p]+WIDTH, y[p], i, j);
        if (distance(x[p]-WIDTH, y[p], i, j) < mindist) mindist = distance(x[p]-WIDTH, y[p], i, j);
      }

     uint8_t coll = 255-(5.4f*mindist);
     if(colour) framebuffer[(2*i)+(2*j)*ARCADA_TFT_WIDTH] = color565(coll, coll<<1, coll<<2);
     else framebuffer[(2*i)+(2*j)*ARCADA_TFT_WIDTH] = color565(coll, coll, coll);

    }
  }
  
  for (int p=0; p<PARTICLES; p++) {
    x[p] += dx[p];
    y[p] += dy[p];
    x[p] = x[p] % WIDTH;
    y[p] = y[p] % HEIGHT;
  }

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
