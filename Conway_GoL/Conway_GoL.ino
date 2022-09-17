// Conway's game of life cellular automata //

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

#define WIDTH  120
#define HEIGHT 120
#define SCR (ARCADA_TFT_WIDTH * ARCADA_TFT_HEIGHT)

  bool grid[2][WIDTH][HEIGHT];
  int current;
    
void rndrule() {

  memset(framebuffer, 0, 2*SCR);

  for (int y = 0; y < HEIGHT; y++){
    
    for (int x = 0; x < WIDTH; x++) grid[0][x][y] = rand()%2;
  
  }

}

void rungrid(){
  
  int count;
  int value = 0;
  int new_grid;

  new_grid = 1 - current;
  
  for (int y = 0; y < HEIGHT; y++) {
  
    for (int x = 0; x < WIDTH; x++) {
      
      count = neighbours(x, y);
      
      if (count < 2 || count > 3) { value = 0; }
      else if (count == 3) { value = 3; }
      else { value = grid[current][x][y]; }
    
      grid[new_grid][x][y] = value;

      if(grid[current][x][y]) framebuffer[(2*x)+(2*y)*ARCADA_TFT_WIDTH] = WHITE;
      else framebuffer[(2*x)+(2*y)*ARCADA_TFT_WIDTH] = BLACK;
    
    }
  }
  
  current = new_grid;
}


int neighbours(int x, int y){
  
  int i, j;
  int result = 0;

  x--;
  y--;
  
  for (i = 0; i < 3; i++) {
  
    if (y < 0 || y > (HEIGHT - 1)) continue;

    for (j = 0; j < 3; j++) {
      if (x < 0 || x > (WIDTH - 1)) continue;
      if (i==1 && j == 1) { x++; continue; }
      if (grid[current][x][y]) result++;
      x++;
    }
    y++;
    x -= 3;
  }
  
  return result;
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

  rungrid();

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
