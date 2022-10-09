// StarWars cellular automata //

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

#define ALIVE     3
#define DEATH_1   2
#define DEATH_2   1
#define DEAD      0

  uint8_t current[SCR];
  uint8_t next[SCR];
  uint8_t alive_counts[SCR];
  uint8_t swap[SCR];

void rndrule(){

  memset(framebuffer, 0, 2*SCR2);

  for(int i = 0; i < SCR; i++){
    current[i] = (rand()%100) < 20 ? ALIVE : DEAD;
    alive_counts[i] = 0;
  }
}

void step(){

  for(int y = 0; y < HEIGHT; y++){
    for(int x = 0; x < WIDTH; x++){
    
      int count = 0;
      int next_val;   
      int mx = WIDTH-1;
      int my = HEIGHT-1;    
      int self = current[x+y*WIDTH];
    
      for (int nx = x-1; nx <= x+1; nx++) { 
        for (int ny = y-1; ny <= y+1; ny++) { 
          if (nx == x && ny == y) continue;    
          if (current[wrap(nx, mx) + wrap(ny, my) * WIDTH] == ALIVE) count++;    
        }   
      }  

      int neighbors = count;
      
      if (self == ALIVE) next_val = ((neighbors == 3) || (neighbors == 4) || (neighbors == 5)) ? ALIVE : DEATH_1;
      else if (self == DEAD) next_val = (neighbors == 2) ? ALIVE : DEAD;
      else next_val = self-1;
   
      next[x+y*WIDTH] = next_val;
  
      if (next_val == ALIVE) alive_counts[x+y*WIDTH] = min(alive_counts[x+y*WIDTH]+1, 100);
      else if (next_val == DEAD) alive_counts[x+y*WIDTH] = 0;
    }
  }
    
  memcpy(swap, current, sizeof(swap));
  memcpy(current, next, sizeof(current));
  memcpy(next, swap, sizeof(next));

}
  
int wrap(int v, int m){

  if (v < 0) return v + m;
  else if (v >= m) return v - m;
  else return v;

}

void draw_type(int min_alive, int max_alive, uint16_t image){  

  for(int y = 0; y < HEIGHT; y++){  
    for(int x = 0; x < WIDTH; x++){
      
      uint16_t coll;
      int self = current[x+y*WIDTH];
      if (self == DEAD) continue;
      int alive = alive_counts[x+y*WIDTH];
      if (alive < min_alive || alive > max_alive) continue;   
      if (self == ALIVE) coll = image;
      else if (self == DEATH_1) coll = GREEN;
      else if (self == DEATH_2) coll = BLACK; 
      framebuffer[(2*x) + (2*y) * ARCADA_TFT_WIDTH] = coll;
   
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
  
  step();

  draw_type(50, 100, RED);
  draw_type(2, 49, BLUE);
  draw_type(0, 1, WHITE);

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
