// Fizzy 2D cellular automata //

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
#define HEIGHT  120
#define SCR     (ARCADA_TFT_WIDTH * ARCADA_TFT_HEIGHT)
#define SCR2    (WIDTH*HEIGHT)

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
  float randomf(float minf, float maxf) {return minf + (rand()%(1UL << 31))*(maxf - minf) / (1UL << 31);}
  
  float CellVal[SCR2];
  uint8_t Calm = 233;
  int CellIndex = 0;
  float a, b, c;
    
void rndrule() {

  memset(framebuffer, 0, 2*SCR);

  CellIndex = 0;
  a = randomf(1.9f, 5.5f);
  b = randomf(1.9f, 5.5f);
  c = randomf(1.9f, 5.5f);
  Calm = 16 + rand()%233;
  for (int i = 0; i < SCR2; i++) CellVal[i] = randomf(0.0f, 255.0f);

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

  for (int j = 0; j < HEIGHT; j++) {
  
    for (int i = 0; i < WIDTH; i++) {

      CellIndex = (CellIndex+1)%SCR2;

      uint8_t klimp = CellVal[CellIndex]*a;       
      uint8_t nifna = CellVal[CellIndex]*b;
      uint8_t blugg = CellVal[CellIndex]*c;
      framebuffer[(2*i)+(2*j)*ARCADA_TFT_WIDTH] = color565(klimp, nifna, blugg);

      int below      = (CellIndex+1)%SCR2;
      int above      = (CellIndex+SCR2-1)%SCR2;
      int left       = (CellIndex+SCR2-HEIGHT)%SCR2;
      int right      = (CellIndex+HEIGHT)%SCR2;
      int aboveright = ((CellIndex-1) + HEIGHT + SCR2)%SCR2;
      int aboveleft  = ((CellIndex-1) - HEIGHT + SCR2)%SCR2;
      int belowright = ((CellIndex+1) + HEIGHT + SCR2)%SCR2;
      int belowleft  = ((CellIndex+1) - HEIGHT + SCR2)%SCR2;

      float NeighbourMix = powf((CellVal[left]*CellVal[right]*CellVal[above]*CellVal[below]*CellVal[belowleft]*CellVal[belowright]*CellVal[aboveleft]*CellVal[aboveright]),0.125f);
      CellVal[CellIndex] = fmod((sqrtf(CellVal[CellIndex]*NeighbourMix))+0.5f, Calm);

    }

  }

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
