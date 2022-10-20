// Wolfram 2D cellular automata //

#include "Adafruit_Arcada.h"

#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define DARKGREY        0x7BEF
#define NAVY            0x000F

Adafruit_Arcada arcada;

uint16_t *framebuffer;

#define WIDTH   120
#define HEIGHT  120
#define SCR     (WIDTH * HEIGHT)
#define SCR2    (ARCADA_TFT_WIDTH * ARCADA_TFT_HEIGHT)

  bool state[SCR];
  bool newst[SCR];
  bool rules[10] = {0,0,1,1,1,1,0,0,0,0};
    
 void rndrule() {

  for(int i = 0; i < 10; i++) rules[i] = rand()%2;

  memset(framebuffer, 0, 2*SCR2);
  memset(newst, 0, SCR);
  memset(state, 0, SCR);
  
  state[(WIDTH/2)+(HEIGHT/2)*WIDTH] = 1;
  state[(WIDTH/2)+((HEIGHT/2)-1)*WIDTH] = 1;
  state[((WIDTH/2)-1)+((HEIGHT/2)-1)*WIDTH] = 1;
  state[((WIDTH/2)-1)+(HEIGHT/2)*WIDTH] = 1;

}

uint8_t neighbors(uint8_t x, uint8_t y) {
  
  uint8_t result = 0;

  if(y > 0 && state[x+(y-1)*WIDTH] == 1) result = result + 1;
  if(x > 0 && state[(x-1)+y*WIDTH] == 1) result = result + 1;
  if(x < WIDTH-1 && state[(x+1)+y*WIDTH] == 1) result = result + 1;
  if(y < HEIGHT-1 && state[x+(y+1)*WIDTH] == 1) result = result + 1;
  
  return result;
 
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

void loop() {

  if (arcada.readButtons() & ARCADA_BUTTONMASK_A) rndrule();

  for(int x = 0; x < WIDTH; x++){
    
    for(int y = 0; y < HEIGHT; y++){
           
      uint8_t totalNeighbors = neighbors(x,y);
      uint16_t coll;
            
      if(state[x+y*WIDTH] == 0 && totalNeighbors == 0)      {newst[x+y*WIDTH] = rules[0]; coll = WHITE;}
      else if(state[x+y*WIDTH] == 1 && totalNeighbors == 0) {newst[x+y*WIDTH] = rules[1]; coll = RED;}
      else if(state[x+y*WIDTH] == 0 && totalNeighbors == 1) {newst[x+y*WIDTH] = rules[2]; coll = GREEN;}
      else if(state[x+y*WIDTH] == 1 && totalNeighbors == 1) {newst[x+y*WIDTH] = rules[3]; coll = BLUE;}
      else if(state[x+y*WIDTH] == 0 && totalNeighbors == 2) {newst[x+y*WIDTH] = rules[4]; coll = YELLOW;}
      else if(state[x+y*WIDTH] == 1 && totalNeighbors == 2) {newst[x+y*WIDTH] = rules[5]; coll = DARKGREY;}
      else if(state[x+y*WIDTH] == 0 && totalNeighbors == 3) {newst[x+y*WIDTH] = rules[6]; coll = MAGENTA;}
      else if(state[x+y*WIDTH] == 1 && totalNeighbors == 3) {newst[x+y*WIDTH] = rules[7]; coll = CYAN;}
      else if(state[x+y*WIDTH] == 0 && totalNeighbors == 4) {newst[x+y*WIDTH] = rules[8]; coll = NAVY;}
      else if(state[x+y*WIDTH] == 1 && totalNeighbors == 4) {newst[x+y*WIDTH] = rules[9]; coll = BLACK;}
      
      framebuffer[(2*y)*ARCADA_TFT_WIDTH+(2*x)] = coll;

    }
  }
 
  memcpy(state, newst, SCR);
  
  arcada.blitFrameBuffer(0, 0, true, false);
 
}
