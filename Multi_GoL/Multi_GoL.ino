// Conway's Game of Life - multi rules //

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

  uint8_t universe[WIDTH][HEIGHT];
  uint8_t universePRIME[WIDTH][HEIGHT];

  int cx = 0;
  int cy = 0;
  int cxPrime = 0;
  int cyPrime = 0;
  int neighbors = 0;
  int rule = 0;
    
void rndrule(){

  memset(framebuffer, 0, 2*SCR);

  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      universePRIME[x][y] = 0;
      universe[x][y] = rand()%2; 
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
  if (arcada.readButtons() & ARCADA_BUTTONMASK_B) rule = rand()%4;

  stepFunc();
    
  arcada.blitFrameBuffer(0, 0, true, false);
 
}

void renderUniverse() {

  for (int j=0; j<HEIGHT; j++) {
    for (int i=0; i<WIDTH; i++) {
      uint16_t coll;
      if (universe[i][j] == 0) coll = BLACK;
      else {
        if (universe[i][j] == 1) coll = WHITE;
        else coll  = YELLOW;
      }
      framebuffer[(2*i)+(2*j)*ARCADA_TFT_WIDTH] = coll;
    }
  }
}

void countNeighbors(){
  
  neighbors = 0;
  for (int i=-1; i<2; i++){
    for (int j=-1; j<2; j++){
      if (i != 0 || j != 0){
        cxPrime = (cx+i)%WIDTH;
        cyPrime = (cy+j)%HEIGHT;
        if(abs(cxPrime) != cxPrime) cxPrime += WIDTH;
        if(abs(cyPrime) != cyPrime) cyPrime += HEIGHT;
        if (universe[cxPrime][cyPrime] == 1) neighbors++;
      }
    }
    if(universe[cx][cy] > 1) universePRIME[cx][cy] = 0;

    if(rule == 0) {
      if(universe[cx][cy] != 1){
        if(neighbors == 3)universePRIME[cx][cy] = 1;
        else universePRIME[cx][cy] = 0;
      }
      if(universe[cx][cy] == 1){
        if(neighbors < 2) universePRIME[cx][cy] = 2;
        else{
          if (neighbors > 3)universePRIME[cx][cy] = 2;
          else universePRIME[cx][cy] = 1;
        }
      }
    } else {
      if (rule == 1) {
      if (universe[cx][cy] != 1){
        if (neighbors == 3) universePRIME[cx][cy] = 1;
        else universePRIME[cx][cy] = 0;
      }
      if (universe[cx][cy] == 1){
        if (neighbors < 2) universePRIME[cx][cy] = 1;
        else {
          if (neighbors > 3) universePRIME[cx][cy] = 2;
          else universePRIME[cx][cy] = 1;
        }
      }
    }

      if (rule == 2) {
        if (neighbors > 4) universePRIME[cx][cy] = 1;
        else {
          if (neighbors < 4) {
            if (universe[cx][cy] == 1) universePRIME[cx][cy] = 2;
            else universePRIME[cx][cy] = 0;
          } else {
            if (universe[cx][cy] == 2)universePRIME[cx][cy] = 0;
            else universePRIME[cx][cy] = universe[cx][cy];
          }
        }
      } else {

        if (rule == 3) {
          if (neighbors < 1)universePRIME[cx][cy] = 0;
          else {
            if (neighbors < 5)universePRIME[cx][cy] = 2;
            else universePRIME[cx][cy] = 1;
          }
        }
        
      }
    }
  }
}

void stepFunc(){
  
  for (int i=0; i<WIDTH; i++) {
    for (int j=0; j<HEIGHT; j++) {
      if (abs(i) == i && i < WIDTH) {
        if (abs(j) == j && j < HEIGHT) {
          cx = i;
          cy = j;
        } else {
          cy = j%HEIGHT;
          if (cy < 0) cy += HEIGHT;
        }
      } else {
        cx = i%WIDTH;
        if (cx < 0) cx += WIDTH;
      }
      countNeighbors();
    }
  }

  for(int j=0; j<HEIGHT; j++){ for(int i=0; i<WIDTH; i++) universe[i][j] = universePRIME[i][j]; } 
  
  renderUniverse();
  
}
