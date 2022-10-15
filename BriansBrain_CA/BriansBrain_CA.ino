// Brian's Brain cellular automata //

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
#define SCR     (WIDTH*HEIGHT)
#define SCR2    (ARCADA_TFT_WIDTH * ARCADA_TFT_HEIGHT)

  #define DENSITY     7
  #define READY       0
  #define REFRACTORY  1
  #define FIRING      2

  uint8_t world[WIDTH][HEIGHT];
  uint8_t temp[WIDTH][HEIGHT];

uint8_t weighted_randint(uint8_t true_weight){
  
  uint8_t choice = rand() % 10;  
  if (choice > true_weight) return 1;
  else return 0;

}


uint8_t count_neighbours(uint8_t world[WIDTH][HEIGHT], uint8_t x_pos, uint8_t y_pos){
    
  int cx, cy, cell;
  int count = 0;

  for (int y = -1; y < 2; y++) {
    for (int x = -1; x < 2; x++) {
      cx = x_pos + x;
      cy = y_pos + y;
      if((0 <= cx && cx < WIDTH) && (0 <= cy && cy < HEIGHT)) {
        cell = world[x_pos + x][y_pos + y];
        if (cell == FIRING) count ++;
      }
    }
  }
  return count;
}


void apply_rules(uint8_t world[WIDTH][HEIGHT]){
  
  uint8_t cell, neighbours;

  memcpy(temp, world, SCR);

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++){
      cell = temp[x][y];          
      if (cell == READY) {
        neighbours = count_neighbours(temp, x, y);
        if (neighbours == 2) world[x][y] = FIRING; }
      else if (cell == FIRING) world[x][y] = REFRACTORY;
      else world[x][y] = READY;
    }
  }
}

void rndrule(){

  memset(framebuffer, 0, 2*SCR2);

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++){
      uint8_t r = weighted_randint(DENSITY);
      if (r == 1) world[x][y] = FIRING;
      else world[x][y] = READY;
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

void loop() {

  if (arcada.readButtons() & ARCADA_BUTTONMASK_A) rndrule();
  
  apply_rules(world);
    
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++){
      uint16_t coll;
      if (world[x][y] == FIRING) coll = WHITE;    
      else if (world[x][y] == REFRACTORY) coll = YELLOW;
      else coll = BLACK; 
      framebuffer[(2*x) + (2*y) * ARCADA_TFT_WIDTH] = coll;
    }
  }

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
