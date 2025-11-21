#include <avr/pgmspace.h>
#include <FastLED.h>

// =======================
// BASIC CONFIG
// =======================
#define MATRIX_WIDTH  16
#define MATRIX_HEIGHT 16
#define NUM_LEDS      (MATRIX_WIDTH * MATRIX_HEIGHT)
#define DATA_PIN      5
#define BUTTON_PIN    2

enum Mode {
  MODE_FIRE = 0,
  MODE_FIREWORKS = 1,
  MODE_HEART = 2
};

Mode currentMode = MODE_FIRE;

CRGB leds[NUM_LEDS];

// Heat map for fire (y = 0 is bottom, y = 15 is top in fire-space)
byte heat[MATRIX_HEIGHT][MATRIX_WIDTH];

// =======================
// MATRIX INDEX MAP (SNAKED)
// =======================
const int fullMatrix[MATRIX_HEIGHT][MATRIX_WIDTH] PROGMEM = {
  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
  {31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16},
  {32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47},
  {63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48},
  {64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79},
  {95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80},
  {96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111},
  {127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112},
  {128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143},
  {159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144},
  {160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175},
  {191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176},
  {192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207},
  {223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208},
  {224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239},
  {255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240}
};

// =======================
// HEART GROWTH STAGES
// =======================

bool isHeartPixel(uint8_t frame, uint8_t x, uint8_t y) {
  switch (frame) {
    case 0:
      return false;

    case 1: // Heart01
      if (y == 5 && ((x >= 5 && x <= 6) || (x >= 9 && x <= 10))) return true;
      if (y == 6 && ((x >= 4 && x <= 11))) return true;
      if (y == 7 && ((x >= 4 && x <= 11))) return true;
      if (y == 8 && ((x >= 5 && x <= 10))) return true;
      if (y == 9 && ((x >= 6 && x <= 9))) return true;
      if (y == 10 && ((x >= 7 && x <= 8))) return true;
      return false;

    case 2: // Heart02
      if (y == 4 && ((x >= 5 && x <= 6) || (x >= 9 && x <= 10))) return true;
      if (y == 5 && ((x >= 4 && x <= 11))) return true;
      if (y == 6 && ((x >= 3 && x <= 12))) return true;
      if (y == 7 && ((x >= 3 && x <= 12))) return true;
      if (y == 8 && ((x >= 4 && x <= 11))) return true;
      if (y == 9 && ((x >= 5 && x <= 10))) return true;
      if (y == 10 && ((x >= 6 && x <= 9))) return true;
      if (y == 11 && ((x >= 7 && x <= 8))) return true;
      return false;

    case 3: // Heart03
      if (y == 3 && ((x >= 5 && x <= 6) || (x >= 9 && x <= 10))) return true;
      if (y == 4 && ((x >= 4 && x <= 11))) return true;
      if (y == 5 && ((x >= 3 && x <= 12))) return true;
      if (y == 6 && ((x >= 2 && x <= 13))) return true;
      if (y == 7 && ((x >= 2 && x <= 13))) return true;
      if (y == 8 && ((x >= 2 && x <= 13))) return true;
      if (y == 9 && ((x >= 3 && x <= 12))) return true;
      if (y == 10 && ((x >= 4 && x <= 11))) return true;
      if (y == 11 && ((x >= 5 && x <= 10))) return true;
      if (y == 12 && ((x >= 6 && x <= 9))) return true;
      if (y == 13 && ((x >= 7 && x <= 8))) return true;
      return false;

    case 4: // Heart04
      if (y == 2 && ((x >= 4 && x <= 6) || (x >= 9 && x <= 11))) return true;
      if (y == 3 && ((x >= 3 && x <= 12))) return true;
      if (y == 4 && ((x >= 2 && x <= 13))) return true;
      if (y == 5 && ((x >= 1 && x <= 14))) return true;
      if (y == 6 && ((x >= 1 && x <= 14))) return true;
      if (y == 7 && ((x >= 1 && x <= 14))) return true;
      if (y == 8 && ((x >= 1 && x <= 14))) return true;
      if (y == 9 && ((x >= 2 && x <= 13))) return true;
      if (y == 10 && ((x >= 3 && x <= 12))) return true;
      if (y == 11 && ((x >= 4 && x <= 11))) return true;
      if (y == 12 && ((x >= 5 && x <= 10))) return true;
      if (y == 13 && ((x >= 6 && x <= 9))) return true;
      if (y == 14 && ((x >= 7 && x <= 8))) return true;
      return false;

    case 5: // Heart05
      if (y == 0 && ((x >= 4 && x <= 6) || (x >= 9 && x <= 11))) return true;
      if (y == 1 && ((x >= 3 && x <= 6) || (x >= 9 && x <= 12))) return true;
      if (y == 2 && ((x >= 2 && x <= 13))) return true;
      if (y == 3 && ((x >= 1 && x <= 14))) return true;
      if (y == 4 && ((x >= 0 && x <= 15))) return true;
      if (y == 5 && ((x >= 0 && x <= 15))) return true;
      if (y == 6 && ((x >= 0 && x <= 15))) return true;
      if (y == 7 && ((x >= 0 && x <= 15))) return true;
      if (y == 8 && ((x >= 0 && x <= 15))) return true;
      if (y == 9 && ((x >= 1 && x <= 14))) return true;
      if (y == 10 && ((x >= 2 && x <= 13))) return true;
      if (y == 11 && ((x >= 3 && x <= 12))) return true;
      if (y == 12 && ((x >= 4 && x <= 11))) return true;
      if (y == 13 && ((x >= 5 && x <= 10))) return true;
      if (y == 14 && ((x >= 6 && x <= 9))) return true;
      if (y == 15 && ((x >= 7 && x <= 8))) return true;
      return false;
  }
  return false;
}

// =======================
// BUTTON / MODE HANDLING
// =======================

int lastButtonReading = HIGH;      // because of INPUT_PULLUP
int stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // ms

void updateButtonAndMode() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }
  lastButtonReading = reading;

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != stableButtonState) {
      stableButtonState = reading;

      if (stableButtonState == LOW) {
        if (currentMode == MODE_HEART) {
          currentMode = MODE_FIRE;
        } else {
          currentMode = static_cast<Mode>(currentMode + 1);
        }
      }
    }
  }
}

// Check if we should exit the current animation
bool shouldExitMode(Mode expectedMode) {
  updateButtonAndMode();
  return currentMode != expectedMode;
}

// Delay that still keeps checking the button
void smartDelay(uint16_t ms, Mode expectedMode) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    if (shouldExitMode(expectedMode)) {
      return;
    }
    delay(1);
  }
}

// =======================
// HEART ANIMATION
// =======================
void drawHeartGeneratedOnce() {
  Mode myMode = MODE_HEART;

  for (uint8_t frame = 0; frame <= 5; frame++) {
    if (shouldExitMode(myMode)) return;

    FastLED.clear();
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
      for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
        if (isHeartPixel(frame, x, y)) {
          int index = pgm_read_word(&(fullMatrix[y][x]));
          leds[index] = CRGB::Red;
        }
      }
    }
    
    FastLED.show();
    smartDelay(200, myMode);   // instead of delay(200)
    if (shouldExitMode(myMode)) return;
  }
}

// =======================
// FIREWORKS
// =======================
void doFireworksOnce() {
  Mode myMode = MODE_FIREWORKS;

  int height        = random(4, 12);
  int startPosition = random(3, 13);
  int delayedBy     = random(50, 101);
  int row           = 0;
  int startPositionCopy = startPosition;

  for (int i = 0; i < height; i++) {
    if (shouldExitMode(myMode)) return;

    FastLED.clear();

    int r = random(0, 255);
    int g = random(0, 255);
    int b = random(0, 255);

    row = 15 - i;
    int rowCopy = row;
    int spark = 1;

    leds[pgm_read_word(&(fullMatrix[row][startPosition]))] = CRGB(r, g, b);

    if (i == (height - 1)) {
      leds[pgm_read_word(&(fullMatrix[row][startPosition]))] = CRGB(r, g, b);
      for (int j = 1; j <= 3; j++) {
        if (shouldExitMode(myMode)) return;

        FastLED.clear();
        r = random(0, 255);
        g = random(0, 255);
        b = random(0, 255);

        leds[pgm_read_word(&(fullMatrix[row + spark][startPosition - spark]))] = CRGB(r, g, b);
        leds[pgm_read_word(&(fullMatrix[row + spark][startPosition + spark]))] = CRGB(r, g, b);
        leds[pgm_read_word(&(fullMatrix[row - spark][startPosition - spark]))] = CRGB(r, g, b);
        leds[pgm_read_word(&(fullMatrix[row - spark][startPosition + spark]))] = CRGB(r, g, b);

        leds[pgm_read_word(&(fullMatrix[row + spark][startPosition]))]        = CRGB(r, g, b);
        leds[pgm_read_word(&(fullMatrix[row - spark][startPosition]))]        = CRGB(r, g, b);
        leds[pgm_read_word(&(fullMatrix[row][startPosition + spark]))]        = CRGB(r, g, b);
        leds[pgm_read_word(&(fullMatrix[row][startPosition - spark]))]        = CRGB(r, g, b);

        if (j >= 1 && j < 3) {
          r = random(0, 255);
          g = random(0, 255);
          b = random(0, 255);
          leds[pgm_read_word(&(fullMatrix[rowCopy + j][startPositionCopy]))]     = CRGB(r, g, b);
          leds[pgm_read_word(&(fullMatrix[rowCopy - j][startPositionCopy]))]     = CRGB(r, g, b);
          leds[pgm_read_word(&(fullMatrix[rowCopy][startPositionCopy + j]))]     = CRGB(r, g, b);
          leds[pgm_read_word(&(fullMatrix[rowCopy][startPositionCopy - j]))]     = CRGB(r, g, b);
        }

        
        FastLED.show();
        smartDelay(delayedBy, myMode);
        if (shouldExitMode(myMode)) return;
        spark++;
      }
    }
    
    FastLED.show();
    smartDelay(delayedBy, myMode);
    if (shouldExitMode(myMode)) return;
  }
}

// =======================
// FIRE ANIMATION
// =======================
CRGBPalette16 gPal = HeatColors_p;

void fireStep() {
  const uint8_t cooling  = 55;
  const uint8_t sparking = 120;

  // 1. COOL DOWN
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      heat[y][x] = qsub8(heat[y][x],
                         random8(0, ((cooling * 10) / MATRIX_HEIGHT) + 2));
    }
  }

  // 2. HEAT DIFFUSES UP (higher y gets heat from below)
  for (int y = MATRIX_HEIGHT - 1; y >= 2; y--) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      heat[y][x] = (heat[y - 1][x] + heat[y - 2][x] + heat[y - 2][x]) / 3;
    }
  }

  // 3. NEW HEAT AT BOTTOM (y = 0)
  for (int x = 0; x < MATRIX_WIDTH; x++) {
    if (random8() < sparking) {
      uint8_t y = 0;
      heat[y][x] = qadd8(heat[y][x], random8(160, 255));
    }
  }

  // 4. MAP HEAT TO COLORS (overwrite all pixels)
  FastLED.clear();
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      uint8_t h = heat[y][x];
      if (y == 1) {
        h = (uint8_t)((heat[1][x] + heat[0][x]) / 2);
      } else if (y == 2) {
        h = (uint8_t)((heat[2][x] + heat[1][x]) / 2);
      }

      uint8_t colorIndex = scale8(h, 240);
      if (colorIndex > 220) colorIndex = 220;

      CRGB color = ColorFromPalette(gPal, colorIndex);

      int matrixRow = MATRIX_HEIGHT - 1 - y;
      int index     = pgm_read_word(&(fullMatrix[matrixRow][x]));
      leds[index] = color;
    }
  }

  
  FastLED.show();
}

// =======================
// SETUP & LOOP
// =======================
void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(64);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  randomSeed(analogRead(A0));

  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      heat[y][x] = 0;
    }
  }

  FastLED.clear();
  FastLED.show();
}

void loop() {
  updateButtonAndMode();  // always track button at top of loop

  switch (currentMode) {
    case MODE_FIRE:
      fireStep();
      smartDelay(100, MODE_FIRE);   // responsive fire mode
      break;

    case MODE_FIREWORKS:
      doFireworksOnce();            // abortable via smartDelay/shouldExitMode
      break;

    case MODE_HEART:
      drawHeartGeneratedOnce();     // abortable via smartDelay/shouldExitMode
      break;
  }
}
