// Thorn fractal //

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
#define ITER    6

float randomf(float minf, float maxf) {return minf + (rand()%(1UL << 31))*(maxf - minf) / (1UL << 31);}
  
  float escapeValue = 10.0f;
  uint16_t coll[ITER];
  int k;
  float xmin = -PI * 0.5f;
  float xmax = +PI * 0.5f;
  float ymin = -PI;
  float ymax = +PI;

  float cr, ci, cnt;
    
void rndrule() {

  memset(framebuffer, 0, 2*SCR);

  escapeValue = randomf(10.0f, 500.0f);
  ci = randomf(-2.0f, 2.0f);
  cr = randomf(-2.0f, 2.0f);
  
  for (int i=0; i<ITER; i++) coll[i] = rand();

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

  float ir, ii, zi, zr, a, b;
 
  float cx = cr + cosf(cnt);
  float cy = ci + sinf(cnt);
  
  cnt = cnt + 0.1f;

  for (int y=0; y < HEIGHT; y++){
    
    zr = ymin + y * (ymax - ymin) / HEIGHT;
    
    for (int x=0; x<WIDTH; x++){
      
      zi = xmin + x * (xmax - xmin) / WIDTH;
      ir = zr;
      ii = zi;
      
      for (k=0; k < ITER; k++){
        
        a = ir;
        b = ii;

        ir = a / cosf(b) + cx;
        ii = b / sinf(a) + cy;

        if (ir*ir + ii*ii > escapeValue) break;
        
      }
      
      framebuffer[x+y*ARCADA_TFT_WIDTH] = coll[k];

    }    
  }

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
