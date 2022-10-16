// Gray-Scott reaction-diffusion //

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

#define WIDTH   60
#define HEIGHT  60
#define SCR     (WIDTH * HEIGHT)
#define ITER    64

uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
float randomf(float minf, float maxf) {return minf + (rand()%(1UL << 31))*(maxf - minf) / (1UL << 31);}
  
  float diffU = 0.16f;
  float diffV = 0.08f;
  float paramF = 0.035f;
  float paramK = 0.06f;
  bool colour = false;
  float U[WIDTH][HEIGHT];
  float V[WIDTH][HEIGHT];
  float dU[WIDTH][HEIGHT];
  float dV[WIDTH][HEIGHT];
    
 void rndrule() {

  diffU = randomf(0.0999f, 0.1999f);
  diffV = randomf(0.0749f, 0.0849f);
  paramF = randomf(0.0299f, 0.0399f);
  paramK = randomf(0.0549f, 0.0649f);

  memset(framebuffer, 0, 2*SCR);
  
  for (int j = 0; j < HEIGHT; j++) { 
    for (int i = 0; i < WIDTH; i++) {    
      U[i][j] = 1.0f;
      V[i][j] = 0.0f;        
    }
  }

  for(int j=(HEIGHT-4)/2;j<(HEIGHT+4)/2;++j){      
    for(int i=(WIDTH-4)/2;i<(WIDTH+4)/2;++i){          
      U[i][j] = randomf(0.5f, 1.0f);
      V[i][j] = randomf(0.25f, 0.5f); 
    }
  }

}

void timestep(float F, float K, float diffU, float diffV) {

  for (int j = 1; j < HEIGHT-1; j++) {
    for (int i = 1; i < WIDTH-1; i++) {
            
      float u = U[i][j];
      float v = V[i][j];      
      float uvv = u * v * v;   
      float lapU = (U[i-1][j] + U[i+1][j] + U[i][j-1] + U[i][j+1] - 4.0f * u);
      float lapV = (V[i-1][j] + V[i+1][j] + V[i][j-1] + V[i][j+1] - 4.0f * v);
          
      dU[i][j] = diffU*lapU - uvv + F*(1.0f-u);
      dV[i][j] = diffV*lapV + uvv - (K+F)*v;
      U[i][j] += dU[i][j];
      V[i][j] += dV[i][j];

      uint8_t coll = 255.0f * U[i][j];
      if(colour) framebuffer[(4*i)+(4*j)*ARCADA_TFT_WIDTH] = color565(coll, coll<<1, coll<<2);
      else framebuffer[(4*i)+(4*j)*ARCADA_TFT_WIDTH] = color565(coll, coll, coll);
          
    }
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

  for (int k = 0; k < ITER; k++) timestep(paramF, paramK, diffU, diffV);
  
  arcada.blitFrameBuffer(0, 0, true, false);
 
}
