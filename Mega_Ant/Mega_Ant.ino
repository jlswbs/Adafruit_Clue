// Mega Langtons ant cellular automata //

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

#define LENGHT  32
#define ITER    2000

  uint16_t antX = WIDTH/2;
  uint16_t antY = HEIGHT/2;
  uint16_t direction;
  uint16_t stateCount;
  bool type[LENGHT];
  uint16_t stateCols[LENGHT];
    
void rndrule() {

  memset(framebuffer, 0, 2*SCR);

  antX = WIDTH/2;
  antY = HEIGHT/2;
  
  stateCount = 2 + rand()%(LENGHT-2);
  direction = rand()%4;

  for(int i = 0; i < stateCount; i++) type[i] = rand()%2;
  for(int i = 0; i < stateCount; i++) stateCols[i] = rand();

}

void turn(int angle){
  
  if(angle == 0){
    if(direction == 0){
      direction = 3;
    } else {
      direction--;
    }
  } else {
    if(direction == 3){
      direction = 0;
    } else {
      direction++;
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

  for(int i = 0; i < ITER; i++) {

    if(direction == 0){
      antY--;
      antX++;
    }

    if(direction == 1){
      antX++;
      antY++;
    }

    if(direction == 2){
      antY++;
      antX--;
    }

    if(direction == 3){
      antX--;
      antY--;
    }

    if (antX > WIDTH-1) { memset(framebuffer, 0, 2*SCR); antX = WIDTH/2; }
    if (antX < 0) { memset(framebuffer, 0, 2*SCR); antX = WIDTH/2; }
    if (antY > HEIGHT-1) { memset(framebuffer, 0, 2*SCR); antY = HEIGHT/2; }
    if (antY < 0) { memset(framebuffer, 0, 2*SCR); antY = HEIGHT/2; }
  
    turn(type[framebuffer[antX+antY*ARCADA_TFT_WIDTH]%stateCount]);
    framebuffer[antX+antY*ARCADA_TFT_WIDTH] = stateCols[framebuffer[antX+antY*ARCADA_TFT_WIDTH]%stateCount];

  }

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
