// Belousov-Zabotinsky reaction //

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
#define SCR     (ARCADA_TFT_WIDTH * ARCADA_TFT_HEIGHT)
#define SCR2    (WIDTH*HEIGHT)

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
  float randomf(float minf, float maxf) {return minf + (rand()%(1UL << 31))*(maxf - minf) / (1UL << 31);}
  
  float a [WIDTH][HEIGHT][2];
  float b [WIDTH][HEIGHT][2];
  float c [WIDTH][HEIGHT][2];
  float adjust = 1.2f;
  bool p = 0, q = 1;
    
void rndrule() {

  memset(framebuffer, 0, 2*SCR);

  adjust = randomf(0.75f, 1.35f);

   for (int y = 0; y < HEIGHT; y++) {
    
    for (int x = 0; x < WIDTH; x++) {
      
      a[x][y][p] = (rand()%10)/10.0f;
      b[x][y][p] = (rand()%10)/10.0f;
      c[x][y][p] = (rand()%10)/10.0f;
      
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

  for (int y = 0; y < HEIGHT; y++) {

    for (int x = 0; x < WIDTH; x++) {
  
      float c_a = 0;
      float c_b = 0;
      float c_c = 0;
    
      for (int i = x - 1; i <= x+1; i++) {

        for (int j = y - 1; j <= y+1; j++) {

          c_a += a[(i+WIDTH)%WIDTH][(j+HEIGHT)%HEIGHT][p];
          c_b += b[(i+WIDTH)%WIDTH][(j+HEIGHT)%HEIGHT][p];
          c_c += c[(i+WIDTH)%WIDTH][(j+HEIGHT)%HEIGHT][p];
        }
      }
      
      c_a /= 9.0f;
      c_b /= 9.0f;
      c_c /= 9.0f;

      a[x][y][q] = constrain(c_a + c_a * (adjust * c_b - c_c), 0.0f, 1.0f);
      b[x][y][q] = constrain(c_b + c_b * (c_c - adjust * c_a), 0.0f, 1.0f);
      c[x][y][q] = constrain(c_c + c_c * (c_a - c_b), 0.0f, 1.0f);
    
      uint8_t l = 255 * a[x][y][q];
      uint8_t r = 255 * b[x][y][q];
      uint8_t s = 255 * c[x][y][q];
      framebuffer[(4*y) * ARCADA_TFT_WIDTH + (4*x)] = color565(l, r, s);
    
    }
  }

  if (p == 0) { p = 1; q = 0; } else { p = 0; q = 1; }

  arcada.blitFrameBuffer(0, 0, true, false);
 
}
