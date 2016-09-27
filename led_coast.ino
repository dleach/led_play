#include <FastLED.h>

//#define LED_PIN     2
#define LED_PIN     14
#define NUM_LEDS    12
//#define BRIGHTNESS  64
#define BRIGHTNESS  200
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_STRIPS  5
#define NUM_LEDSx 5

CRGB leds[NUM_LEDS * NUM_STRIPS];
//CRGB leds[NUM_LEDS];

// This is the temporary holding area for the led strip values
CRGB sleds[NUM_STRIPS];

#define UPDATES_PER_SECOND 100

// This example shows several ways to set up and use 'palettes' of colors
// with FastLED.
//
// These compact palettes provide an easy way to re-colorize your
// animation on the fly, quickly, easily, and with low overhead.
//
// USING palettes is MUCH simpler in practice than in theory, so first just
// run this sketch, and watch the pretty lights as you then read through
// the code.  Although this sketch has eight (or more) different color schemes,
// the entire sketch compiles down to about 6.5K on AVR.
//
// FastLED provides a few pre-configured color palettes, and makes it
// extremely easy to make up your own color schemes with palettes.
//
// Some notes on the more abstract 'theory and practice' of
// FastLED compact palettes are at the bottom of this file.



CRGBPalette16 currentPalette;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

/*
 * I want to treat each strip as a single pixel (which happens to have 12 LEDs). 
 * 
 * - parameterize the code such that I have a "number of strips" and have a CRGB array
 *   to represent each strip as a single pixel. Then copy the CRGB into the strips CRGB 
 *   array (FillLEDsFromPaletteColors()). 
 *   
 *   So the palete functions will use the number of strips count to think it is filling 
 */
//#define STRIP_TEST  1 
void setup() {
    delay( 3000 ); // power-up safety delay
    
    random16_set_seed(8934);
    random16_add_entropy(analogRead(3));

    Serial.begin(9600);
    
    // With the Teensy the LED strips are 
    // 2  LED Strip #1
    // 14  LED Strip #2
    // 7 LED Strip #3
    // 8 LED Strip #4
    // 6 LED Strip #5
    // 20  LED Strip #6
    // 21  LED Strip #7
    // 5 LED Strip #8
    // 15 & 16 Connect together
    // 4 Do not use
    // 3 Do not use as PWM
    // 12  Frame Sync (Video)
#if STRIP_TEST == 1
    Serial.println("Strip test...");
    FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(sleds, NUM_STRIPS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, 14, COLOR_ORDER>(sleds, NUM_STRIPS).setCorrection( TypicalLEDStrip );
#else    
#if (NUM_STRIPS > 0)
    Serial.println("Strip 1...");
    FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(&leds[0], NUM_LEDS).setCorrection( TypicalLEDStrip );
#endif    
#if (NUM_STRIPS > 1)
    Serial.println("Strip 2...");
    FastLED.addLeds<LED_TYPE, 14, COLOR_ORDER>(&leds[1 * NUM_LEDS], NUM_LEDS).setCorrection( TypicalLEDStrip );
#endif    
#if (NUM_STRIPS > 2)
    Serial.println("Strip 3...");
    FastLED.addLeds<LED_TYPE, 7, COLOR_ORDER>(&leds[2 * NUM_LEDS], NUM_LEDS).setCorrection( TypicalLEDStrip );
#endif    
#if (NUM_STRIPS > 3)
    Serial.println("Strip 4...");
    FastLED.addLeds<LED_TYPE, 8, COLOR_ORDER>(&leds[3 * NUM_LEDS], NUM_LEDS).setCorrection( TypicalLEDStrip );
#endif    
#if (NUM_STRIPS > 4)
    Serial.println("Strip 5...");
    FastLED.addLeds<LED_TYPE, 6, COLOR_ORDER>(&leds[4 * NUM_LEDS], NUM_LEDS).setCorrection( TypicalLEDStrip );
#endif    
#if (NUM_STRIPS > 5)
    Serial.println("Strip 6...");
    FastLED.addLeds<LED_TYPE, 20, COLOR_ORDER>(&leds[5 * NUM_LEDS], NUM_LEDS).setCorrection( TypicalLEDStrip );
#endif    
#if (NUM_STRIPS > 6)
    Serial.println("Strip 7...");
    FastLED.addLeds<LED_TYPE, 21, COLOR_ORDER>(&leds[6 * NUM_LEDS], NUM_LEDS).setCorrection( TypicalLEDStrip );
#endif    
#if (NUM_STRIPS > 7)
    Serial.println("Strip 8...");
    FastLED.addLeds<LED_TYPE, 5, COLOR_ORDER>(&leds[7 * NUM_LEDS], NUM_LEDS).setCorrection( TypicalLEDStrip );
#endif    
#endif

    FastLED.setBrightness(  BRIGHTNESS );
    
    currentPalette = RainbowColors_p;
}



typedef void (*SimplePatternList)(struct CRGB* leds, uint16_t num_leds);
typedef struct {
  SimplePatternList pattern;
  uint16_t           min_sec;
  uint16_t           max_sec;
} pat_time_t;


pat_time_t gPatterns[] = {
  {rainbow, 30, 120},
  {rainbowWithGlitter , 10, 30},
  {confetti, 10, 30},
  {sinelon, 30, 60},
  {totalrandom, 30, 90},
  {totalrandom_ocean, 30, 120},
  {totalrandom_lava, 30, 120},
  {totalrandom_forest, 30, 120},
  {lava, 30, 90},
  {lavaWithGlitter, 30, 75},
  {forest, 30, 120},
  {sinelon_slow, 30, 90},
  {tracer, 30, 90},
  {juggle, 20, 120},
  {ocean, 30, 120},
  {bpm, 10, 35},
  };


uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
CEveryNSeconds mytime(gPatterns[gCurrentPatternNumber].min_sec);

uint8_t patternNameDisplayed = 0;

uint8_t mybrightness = 0;
#define BRIGHT_SCALE 1

void loop()
{
    //Serial.println("Hello World...");
    //fill_solid(leds, NUM_LEDS, CRGB::Red);
    //fill_solid(sleds, NUM_LEDS, CRGB::White);
    //FastLED.show();
    //FastLED.delay(1000/UPDATES_PER_SECOND);

    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber].pattern(sleds, NUM_STRIPS);
    mybrightness = (mybrightness > (255 - BRIGHT_SCALE)) ? 255 : (mybrightness + BRIGHT_SCALE);
    setbrightness(sleds, NUM_STRIPS, mybrightness);
    
    // Translate over to my strings
    translateToStrings(sleds, leds, NUM_STRIPS, NUM_LEDS);

    // Show them
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);

    
    // do some periodic updates
    EVERY_N_MILLISECONDS( 100 ) { gHue++; } // slowly cycle the "base color" through the rainbow

    if (mytime)
    {
      // change patterns periodically
      while (mybrightness != 0)
      {
        mybrightness = (mybrightness < BRIGHT_SCALE) ? 0 : (mybrightness - BRIGHT_SCALE);
        gPatterns[gCurrentPatternNumber].pattern(sleds, NUM_STRIPS);
        
        setbrightness(sleds, NUM_LEDSx, mybrightness);
        translateToStrings(sleds, leds, NUM_STRIPS, NUM_LEDS);

        FastLED.show();
        FastLED.delay(1000 / UPDATES_PER_SECOND);
        EVERY_N_MILLISECONDS( 200 ) { gHue++; } // slowly cycle the "base color" through the rainbow
      }
      delay(1000);  // Stay black for a bit... 1 sec
      //Serial.println("Switching pattern...");
      
      nextPattern();
      patternNameDisplayed = 0;
      mytime.setPeriod(random(gPatterns[gCurrentPatternNumber].min_sec,gPatterns[gCurrentPatternNumber].max_sec ));     
      Serial.println(mytime.getPeriod(), DEC);
    }
}
/************/
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void translateToStrings(struct CRGB* sleds, struct CRGB* leds, uint16_t num_sleds, uint16_t num_leds_per)
{
#if STRIP_TEST == 1  
#else  

    for( int str_idx = 0; str_idx < num_sleds; str_idx++)
    {
      #if 0
        if (!sleds[str_idx])
        {
          Serial.print("BLACK: ");
          Serial.print(sleds[str_idx].r,DEC);
          Serial.print(" ");
          Serial.print(sleds[str_idx].g,DEC);
          Serial.print(" ");
          Serial.println(sleds[str_idx].b,DEC);
          
          Serial.print(sleds[str_idx-1].r,DEC);
          Serial.print(" ");
          Serial.print(sleds[str_idx-1].g,DEC);
          Serial.print(" ");
          Serial.println(sleds[str_idx-1].b,DEC);
          Serial.println(gHue, DEC);

        }
       #endif
        for( int led_idx = 0; led_idx < num_leds_per; led_idx++)
        {
          //Serial.println((num_leds_per*str_idx)+led_idx, DEC);
          leds[(num_leds_per*str_idx)+led_idx] = sleds[str_idx];
        }
    }
#endif
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);

  gCurrentPatternNumber = random(ARRAY_SIZE(gPatterns));
  //gCurrentPatternNumber = 3;
}



void rainbow(struct CRGB* leds, uint16_t num_leds) 
{
  if (!patternNameDisplayed)
  {
      Serial.println("rainbow pattern...");
      patternNameDisplayed = 1;
  }
  
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, num_leds, gHue, 7);

  // Lets fix black
  if (num_leds > 1)
  {
    for (int i=0; i < num_leds; i++)
    {
      if (!leds[i])
      {
        leds[i] = (i == 0) ? leds[i+1] : leds[i-1];
      }
    }
  }
}

void fadeblack(struct CRGB* leds, uint16_t num_leds)
{
  fadeToBlackBy( leds, num_leds, 10);
}

void setbrightness( struct CRGB* leds, uint16_t num_leds, uint8_t brightness)
{
    
    if( brightness != 255) {
        for (int i=0; i < num_leds; i++)
        {
          
            if( brightness ) {
                brightness++; // adjust for rounding
                // Now, since brightness is nonzero, we don't need the full scale8_video logic;
                // we can just to scale8 and then add one (unless scale8 fixed) to all nonzero inputs.
                if( leds[i].red )   {
                    leds[i].red = scale8_LEAVING_R1_DIRTY( leds[i].red, brightness);
    #if !(FASTLED_SCALE8_FIXED==1)
                    leds[i].red++;
    #endif
                }
                if( leds[i].green ) {
                    leds[i].green = scale8_LEAVING_R1_DIRTY( leds[i].green, brightness);
    #if !(FASTLED_SCALE8_FIXED==1)
                    leds[i].green++;
    #endif
                }
                if( leds[i].blue )  {
                    leds[i].blue = scale8_LEAVING_R1_DIRTY( leds[i].blue, brightness);
    #if !(FASTLED_SCALE8_FIXED==1)
                    blue1++;
    #endif
                }
                cleanup_R1();
            } else {
                leds[i].red = 0;
                leds[i].green = 0;
                leds[i].blue = 0;
            }
        }
    }
}

void ocean(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("ocean pattern...");
      patternNameDisplayed = 1;
  }
  for (int i = 0; i < num_leds; i++)
  {
    leds[i] = ColorFromPalette( OceanColors_p, gHue+(i*3), BRIGHTNESS, LINEARBLEND);
  }
}

void lava(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("lava pattern...");
      patternNameDisplayed = 1;
  }
  for (int i = 0; i < num_leds; i++)
  {
    leds[i] = ColorFromPalette( LavaColors_p, gHue, BRIGHTNESS, LINEARBLEND);
  }
}

void lavaWithGlitter(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("lava with glitter pattern...");
      patternNameDisplayed = 1;
  }
  lava(leds, num_leds);
  addGlitter(leds, num_leds, 3);
}

void forest(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("forest pattern...");
      patternNameDisplayed = 1;
  }
  for (int i = 0; i < num_leds; i++)
  {
    leds[i] = ColorFromPalette( ForestColors_p, gHue, BRIGHTNESS, LINEARBLEND);
  }
}

void rainbowWithGlitter(struct CRGB* leds, uint16_t num_leds) 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  if (!patternNameDisplayed)
  {
      Serial.println("rainbow with glitter pattern...");
      patternNameDisplayed = 1;
  }
  rainbow(leds, num_leds);
  addGlitter(leds, num_leds, 20);
}

void addGlitter( struct CRGB* leds, uint16_t num_leds, fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(num_leds) ] += CRGB::White;
  }
}

void confetti(struct CRGB* leds, uint16_t num_leds) 
{
  // random colored speckles that blink in and fade smoothly
  if (!patternNameDisplayed)
  {
      Serial.println("confetti pattern...");
      patternNameDisplayed = 1;
  }
  fadeToBlackBy( leds, num_leds, 10);
  int pos = random(num_leds);
  leds[pos] += CHSV( gHue + random(64), 200, 255);
}

void sinelon_supp(struct CRGB* leds, uint16_t num_leds, accum88 bpm)
{
  // a colored dot sweeping back and forth, with fading trails
  // Smaller the number the longer the fade and trail
  fadeToBlackBy( leds, num_leds, (bpm/2));
  int pos = beatsin16(bpm,0,num_leds-1);
  leds[pos] += CHSV( gHue, 255, 192);
}

void sinelon(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("sine pattern...");
      patternNameDisplayed = 1;
  }
  sinelon_supp(leds, num_leds, 20);
}

void sinelon_slow(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("sine slow pattern...");
      patternNameDisplayed = 1;
  }
 sinelon_supp(leds, num_leds, 8);
}

/*
 * The problem with using the beatsin16() type function is that the sin16 
 * returns -32767 to 32767 but then the beatsin16() adds 32768 to force it 
 * positive... haven't fully thought that through but there does seem to be 
 * a tendency to stick to one side for a moment but I think this then causes
 * the number to be at the ends of the scale16() twice as long because of the
 * oscillating nature of the number ( 0->64K then 64K->0). So the scale16() 
 * will give the end points longer staying time.
 * .
 */
void tracer_supp(struct CRGB* leds, uint16_t num_leds, accum88 bpm)
{
  // a colored dot sweeping back and forth, with fading trails
  // Smaller the number the longer the fade and trail
  fadeToBlackBy( leds, num_leds, (bpm/3));
  
  static int tracerdir = 1;
  static int tracerpos = 0;
  static uint16_t lastbeat = 0; 
  
  uint16_t tmp = beat16(bpm,0);
  tmp = scale16(tmp, num_leds-1);
  //tmp = beatsin16(bpm,0,num_leds-1);
  if (tmp != lastbeat)
  {
    lastbeat = tmp;
    if (random(100) > 90)
    {
      tracerdir = -tracerdir;
    }
    tracerpos += tracerdir;
    if (tracerpos < 0) 
    {
      tracerpos = num_leds-1;
    }
    if (tracerpos == num_leds)
    {
      tracerpos = 0;
    }
  }
  leds[tracerpos] += CHSV( gHue, 255, 192);
}

void tracer(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("tracer pattern...");
      patternNameDisplayed = 1;
  }
  tracer_supp(leds, num_leds, 10);
}

void random_supp(struct CRGB* leds, uint16_t num_leds, uint16_t x)
{
  fadeToBlackBy( leds, num_leds, (x/10) + 1);
  if (random8(x*10)==1)
  {
    int pos = random8(num_leds);
    //leds[pos] += CHSV( gHue, 255, 192);
    leds[pos] = ColorFromPalette( OceanColors_p, random8(), BRIGHTNESS, LINEARBLEND);
  }
  
}

void totalrandom(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("total random pattern...");
      patternNameDisplayed = 1;
  }
  random_supp(leds, num_leds, 10);
}

void random_palette_supp(struct CRGB* leds, uint16_t num_leds, const CRGBPalette16& pal, uint16_t x)
{
  fadeToBlackBy( leds, num_leds, (x/10) + 1);
  if (random8(x*10)==1)
  {
    int pos = random8(num_leds);
    leds[pos] = ColorFromPalette( pal, random8(), BRIGHTNESS, LINEARBLEND);
  }
  
}

void totalrandom_ocean(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("random ocean pattern...");
      patternNameDisplayed = 1;
  }
  random_palette_supp(leds, num_leds, OceanColors_p, 10);
}

void totalrandom_lava(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("random lava pattern...");
      patternNameDisplayed = 1;
  }
  random_palette_supp(leds, num_leds, LavaColors_p, 10);
}

void totalrandom_forest(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("random forest pattern...");
      patternNameDisplayed = 1;
  }
  random_palette_supp(leds, num_leds, ForestColors_p, 10);
}

void bpm(struct CRGB* leds, uint16_t num_leds)
{
  if (!patternNameDisplayed)
  {
      Serial.println("bpm pattern...");
      patternNameDisplayed = 1;
  }
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < num_leds; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle(struct CRGB* leds, uint16_t num_leds) {
  if (!patternNameDisplayed)
  {
      Serial.println("juggle pattern...");
      patternNameDisplayed = 1;
  }
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, num_leds, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,num_leds)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

/**************/



// Additionl notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact 
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved 
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.
