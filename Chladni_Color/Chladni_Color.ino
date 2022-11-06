// Chladni patterns - color //

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

#define WIDTH   120
#define HEIGHT  100
#define SCR     (WIDTH * HEIGHT)
#define SCR2    (ARCADA_TFT_WIDTH * ARCADA_TFT_HEIGHT)
#define NUMS    16

float randomf(float minf, float maxf) {return minf + (rand()%(1UL << 31))*(maxf - minf) / (1UL << 31);}
  
  float p[WIDTH][HEIGHT];
  float v[WIDTH][HEIGHT];
  float a, b, c, d;
  uint16_t color[NUMS];

void rndrule(){

  memset(framebuffer, 0, 2*SCR2);

  a = randomf(0.005f, 0.05f);
  b = randomf(0.005f, 0.05f);
  c = randomf(0.005f, 0.05f);
  d = randomf(1.0f, 12.0f);
  
  for (int i = 0; i < NUMS; i++) color[i] = rand();
  
  for (int y = 0; y < HEIGHT; y++){
    for (int x = 0; x < WIDTH; x++){
      p[x][y] = 0;
      v[x][y] = 0;
    }
  }

}

void setup(void){

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

void loop(){

  if (arcada.readButtons() & ARCADA_BUTTONMASK_A) rndrule();

  int frame = millis() / 32;

  v[WIDTH/2][HEIGHT/2] = 0;
  p[WIDTH/2][HEIGHT/2] = (sinf(frame * a) + sinf(frame * b) + sinf(frame * c)) * d;
  
  for (int y = 1; y < HEIGHT-1; y++) {
    for (int x = 1; x < WIDTH-1; x++) {
      v[x][y] += (p[x-1][y] + p[x+1][y] + p[x][y-1] + p[x][y+1]) * 0.25f - p[x][y];
    }
  }
  
  for (int y = 0; y < HEIGHT; y++){
    for (int x = 0; x < WIDTH; x++){
      p[x][y] += v[x][y];
      uint8_t coll = NUMS * fabs(constrain(p[x][y], -1.0f, 1.0f));
      framebuffer[(2*x)+(20+(2*y))*ARCADA_TFT_WIDTH] = color[coll%NUMS];
    }
  }

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
