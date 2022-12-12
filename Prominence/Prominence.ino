// Prominence math patterns //

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
#define ITER    10
#define N       1000
#define M       5  

float randomf(float minf, float maxf) {return minf + (rand()%(1UL << 31))*(maxf - minf) / (1UL << 31);}

  uint16_t coll = rand();
  float h = 0.5f;
  float ds = -1.0f;
  float X[N];
  float Y[N];
  float T[M];
  float dx, dy, a, s, k;
  int ct;
    
void rndrule() {

  memset(framebuffer, 0, 2*SCR);

  for (int I = 0 ; I < N ; I++){
    X[I] = randomf(0.0f , WIDTH);
    Y[I] = WIDTH;
  }
  
  for (int II = 0 ; II < M ; II++) T[II] = randomf(10.0f, 40.0f);

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

  if(ct == ITER) {
    
    coll = rand();
    ct = 0;
    
  }    

  for (int I = 0; I < N; I++){

    for (int II = 0; II < M; II++){
      
      a = 2.0f * PI * II / M;
      k = (X[I] * cosf(a)) - ((Y[I]+s) * sinf(a));
      k = h * sinf(k/T[II]);
      float kdx = -k * sinf(-a);
      float kdy = +k * cosf(-a);
      dx = dx + kdx;
      dy = dy + kdy;
      
    }
    
    X[I] = X[I] + dx;
    Y[I] = Y[I] + dy;
    dx = 0;
    dy = 0;
    
    if(X[I] < 0 || X[I] > WIDTH || Y[I] > HEIGHT || Y[I] < 0 || randomf(0.0f, 1000.0f) < 1) {
      
      Y[I] = 0.0f;
      X[I] = randomf(0.0f, WIDTH);   
    
    }
    
    a = PI * X[I] / WIDTH;
    k = Y[I] + 10.0f;

    int ax = (WIDTH/2) + (k * sinf(a));
    int bx = (WIDTH/2) - (k * sinf(a));    
    int ay = (HEIGHT/2) + (k * cosf(a));
    
    if(ax>0 && ax<WIDTH && ay>0 && ay<HEIGHT) {      
    
      framebuffer[ax+ay*ARCADA_TFT_WIDTH] = coll;
      framebuffer[bx+ay*ARCADA_TFT_WIDTH] = coll;     

    }
  
  }
  
  s = s + ds;
  ct++;

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
