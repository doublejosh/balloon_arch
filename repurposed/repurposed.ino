#include "SPI.h"
#include "WS2801.h"

/*****************************************************************************
Example sketch for driving WS2801 pixels
*****************************************************************************/

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
int dataPin = 5;
int clockPin = 4;
// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply
int PINS = 50;

boolean pattern_rotate = 0;

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
WS2801 strip = WS2801(PINS, dataPin, clockPin);

static unsigned int masterWait = 30000;

// Allow dynamic functions
typedef void (*function) ();

// Dynamic function list.
// NOTE: patttern (p) limiter in loop().
function arrOfFunctions[2] = {&alternate, &bounce};
//function arrOfFunctions[4] = {&flames, &alternate, &heartBeat, &bounce};

uint32_t c = Color(255, 0, 100);
uint32_t c2 = Color(0, 0, 0);

uint32_t c3 = Color(200, 0, 200);
uint32_t c4 = Color(0, 200, 0);
uint32_t c5 = Color(0, 0, 0);

int dim = 230; // 0 - 255
uint32_t rotate_colors[6] = {
  Color(1*dim     , 0         , 0         ),  // RED
  Color(1*dim     , 0.2706*dim, 0         ),  // ORANGE
  Color(1*dim     , 0.7059*dim, 0         ),  // YELLOW
  Color(0         , 1*dim     , 0         ),  // GREEN
  Color(0         , 0         , 1*dim     ),  // BLUErrrrrrrrrrrrrrr
  Color(0.7157*dim, 0.0255*dim, 0.5647*dim)   // PURPLE
/*
  Color(255, 0, 0),
  Color(225, 110, 25),
  Color(255, 215, 57),
  Color(75, 185, 100),
  Color(45, 150, 235),
  Color(255, 80, 140),
*/
};
int rotate_color_index = 0;

uint8_t wait = 30;

int p = 0;
int priorHeight = 0;

static unsigned long lWaitMillis;

void setup() {
  strip.begin();
  lWaitMillis = millis() + masterWait;
  randomSeed(analogRead(0));
  //Serial.begin(115200);
  //Serial.println("Ready to send data."); 
}


void loop() {
    // Rotate colors.
    c = rotate_colors[rotate_color_index];
    rotate_color_index++;
    if (rotate_color_index >= 6) {
      rotate_color_index = 0;
    }

  if(pattern_rotate) {

    if( (long)( millis() - lWaitMillis ) >= 0) {
      
      //Rollover has occured...
      p++;
      lWaitMillis += masterWait;  // do it again 1 second later
      strip.show();
      
    }
    else {
      
      // Still before the next rollover time...
      arrOfFunctions[p]();

    }
    
    // Keep within available function list.
    if (p >= 2) {
      p = 0;
    }
  }
  else {
   
    //bounce();
    //flames();
    //heartBeat();
    alternate();
    
  }

}


void flames() {
 int i, killBlue;
 
 // Pick a height
 int height = random(0, strip.numPixels());
 
 // Flame colors.
 int colors[10][3] = {
   {255,0,0},
   {170,17,16},
   {235,41,6},
   {170,17,16},
   {255,0,0},
   {235,41,6},
   {255,0,0},
   {170,17,16},
   {235,41,6},
   {170,17,16}
 };
 
 for (i=0;i<strip.numPixels();i++) {
   
   // Normal way reports "error: invalid conversion from 'int*' to 'int'"
   int flame_rgb[3];
   flame_rgb[0] = colors[random(0, 6)][0];
   flame_rgb[1] = colors[random(0, 6)][1];
   flame_rgb[2] = colors[random(0, 6)][2];
   
   // Wipe everything.
   //for (int w = 0; w < strip.numPixels(); w++) {
     //strip.setPixelColor(i, Color(0, 0, 0));
   //}
   
   // Keep off sometimes.
   if(int r = random(0, 5) > 1) {
     

     // Use height.
     if (i < height) {
       
       // Make extra orange fire-ish at the tip.
       if (abs(i - height) < 3) {
         killBlue = 0;
       }
       else {
         killBlue = 1;
       }
     
       // Pick brightness.
       int randNumber = random(0, pow((PINS+1)-i,3));

       strip.setPixelColor(i, Color(
        (int) flame_rgb[0]*(1-1/randNumber),
        (int) flame_rgb[1]*(1-1/randNumber),
        (int) (flame_rgb[2])*(1-1/randNumber) * killBlue
       ));
     }
     else {
       strip.setPixelColor(i, Color(0, 0, 0)); 
     }
     
   }
   else {
     // Wipe when off.
     strip.setPixelColor(i, Color(0, 0, 0));
   }
   
   priorHeight = height;
   
 }
 strip.show();   // write all the pixels out
 delay(wait*1.25);
  
}


void bounce() {
 
  int i;
  boolean dir = true;
  
  for (i=0; i<strip.numPixels(); i=i) {
  
    // Wipe everything.
    for (int c = 0; c < strip.numPixels(); c++) {
      strip.setPixelColor(c, c2);
    }
    
    // Draw pixel.
    strip.setPixelColor(i, c);
    
    if (i == strip.numPixels() - 1) {
      // Change directions.
      dir = !dir;
    }
    
    // Advance position.
    if (dir) {
      i++;
    }
    else {
      i--; 
    }
    
    strip.show();   // write all the pixels out
    
    // Can be used to bounce end to end.
    float mid = i - PINS;
    float amnt = abs(mid);
    delay( (1/pow(amnt , 1.04)) * (wait*15) );
  }
}


// Chasing Pattern.
void alternate() {
  
  int i, j;
  int interval = 7;
  uint8_t wait = 170;
   
  for (j=0; j < interval; j++) {
    for (i=0; i < strip.numPixels(); i++) {
      if (i % interval != j) {
        strip.setPixelColor(i, c2);
      }
      else {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i-1, c);
      }
    }
    
    if (j == interval) {
      j = 0;
    }
    
    strip.show();   // write all the pixels out
    delay(wait*2);
  }
}


// Pumping Heart Pattern.
void heartBeat() {
  
  // Counters for animations.
  int t, i, b = 0, j = 0;
  
  // Configs.
  int interval = 7;   // Cell spacing.
  uint8_t wait = 80;  // Frame timing.
  
  // Heart animation beat frames.
  uint32_t beats[] = {
    Color(200, 0, 120),
    Color(10, 0, 5),
    Color(200, 0, 120),
    Color(70, 0, 50),
    Color(30, 0, 40),
    Color(10, 0, 20),
    Color(0,  0, 0),
    Color(0,  0, 0),
    Color(0,  0, 0),
    Color(0,   0, 0)
  };
  
  // Run the animation.
  for(t=0;t<30;t++) {
    
    // Fill up all the pixels.
    for (i=0; i < strip.numPixels(); i++) {
      
      // Pump the heart.
      if (i>((PINS/2)-2) && i<((PINS/2)+2)) {
        // Slower than blood.
        if (t%3==0) {
          // Use animation beat frame.
          strip.setPixelColor(i, beats[b]);
        }
      }
      else {

        // Run the blood.
        if (i % interval != j) {
          strip.setPixelColor(i, Color(0, 0, 0));
        }
        else {
          // Color the blood correctly.
          int section = i / ((PINS/2)+1);
          uint32_t colors[] = {
            Color(0, 0, 255),
            Color(255, 0, 0),
          };
          // Draw cells.
          strip.setPixelColor(i, colors[section]);
        }

      }
    }
    
    // Increment heart animation, slower than blood.
    if (t%3==0) {
      b++;
      if (b > 8) {b = 0;}
    }
    
    // Increment blood animation.
    j++;
    if (j == interval) {j = 0;}
    
    // Draw.
    strip.show();
    delay(wait*2);
  }

}


/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}
