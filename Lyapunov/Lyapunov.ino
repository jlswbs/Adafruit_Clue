// Lyapunov fractal //

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
#define ITER    5

uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
float randomf(float minf, float maxf) {return minf + (rand()%(1UL << 31))*(maxf - minf) / (1UL << 31);}

  float total, x, r;
  float a1, b1;
  float dx, dy;
  float xd, yd;
  float attractor;
  float MinX;
  float MaxX;
    
void rndrule() {

  memset(framebuffer, 0, 2*SCR);

  attractor = 0.0f;
  MinX = -MAXFLOAT;
  MaxX = MAXFLOAT;
  xd = randomf(1.0f, 1.9f);
  yd = randomf(1.0f, 1.9f);
  dx = randomf(0.5f, 1.0f)/(WIDTH/3);
  dy = randomf(0.5f, 1.0f)/(HEIGHT/3);

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

  uint8_t coll;
  bool pari = true;
  b1 = yd;
  
  for (int a = 0; a < HEIGHT; a++){
    
    b1 = (b1 + dy);
    a1 = xd;
    
    for (int b = 0; b < WIDTH; b++){
      
      a1 = (a1 + dx);
      total = 0;
      x = attractor;

      for (int n = 1; n <= ITER; n++){
        
        if (pari) {
          r = a1;
          pari = false;
        } else{
          r = b1;
          pari = true;
        }
        
        if ((x > MinX) && (x < MaxX)) x =  r*x*(1-x);
        else if (x < MinX) x = fabs(x);
        else x = MaxX * MaxX;

        float temp = fabs(r-2*r*x);
        if (temp != 0) total = total + logf(temp);
        
      }

      float lyap = total / ITER;
      if(lyap >= 0) coll = 0;
      else coll = 128.0f - fabs(128.0f * lyap);
      if(coll < 128) framebuffer[b+a*ARCADA_TFT_WIDTH] = color565(coll, coll, coll);
      
    }
  }

  arcada.blitFrameBuffer(0, 0, true, false);

  attractor = attractor + 0.004f;
  if(attractor > 0.5f) rndrule();
 
}
