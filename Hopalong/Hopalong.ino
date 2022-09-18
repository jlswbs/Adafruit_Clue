// Hopalong orbit fractal //

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
#define SCR     WIDTH * HEIGHT

  #define ITER    10000

  long ranfseed;
  float randf() {return (ranfseed=(ranfseed*1629+1)%1048576)/1048576.0f;}
  float a, b, c, x, y, t;
    
void rndrule() {

  memset(framebuffer, 0, 2*SCR);

  x = 0.1f;
  y = 0.1f;
  t = 0.0f;
  a = 0.0f;
  b = 0.0f;
  c = 0.0f;

  ranfseed = 1 + rand()%SCR;
  
  float pmax = WIDTH, logpmax = logf(pmax);
  
  a = expf(randf()*logpmax);
  b = expf(randf()*logpmax);
  c = randf()*pmax; 

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

  uint16_t coll = rand();

  for (int i=0; i<ITER; i++) {

    float nx = x;
    float ny = y;

    t = sqrtf(fabs(b*nx-c));
    x = ny - ((nx<0) ? t : -t);
    y = a - nx;

    int ax = (WIDTH/2) + (x/2);
    int ay = (HEIGHT/2) + (y/2);
      
    if (ax>0 && ax<WIDTH && ay>0 && ay <HEIGHT) framebuffer[ax+ay*ARCADA_TFT_WIDTH] = coll;

  }

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
