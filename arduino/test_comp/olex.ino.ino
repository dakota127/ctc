//*********************************************************************
// OLED Stuff...
//
// adapted from Adafruit
// Nov. 2019  Peter K. Boxler
//********************************************************************


int Variable1;                    // Create a variable to have something dynamic to show on the display
#include <Fonts/FreeSans9pt7b.h>  // Add a custom font

int16_t  x1, y1;
uint16_t w, h;


//---------------------------------------
void writeTitle() {
 // draw multiple rectangles
  uint8_t color = 1;

   // Clear the buffer.
  display.clearDisplay();

  for (int16_t i=0; i<display.height()/2; i+=3) {
    // alternate colors
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, color%2);
    display.display();
    color++;
  }
  display.display();
  delay(1000);
  display.clearDisplay();

  // tWrite Title
  display.setFont(&FreeSans9pt7b);  // Set a custom font
  display.setTextSize(0);  // Set text size. We are using a custom font so you should always use the text size of 0
//  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,12);
  display.println("Test-Board");
  
}
//---------------------------------------
void writeLine (int line, char* was) {
  
  display.setTextSize(0);
  display.fillRect(0,linepos[line],120,8,0);
  display.display();
  display.setTextColor(WHITE);
  display.setCursor(0,linepos[line]);
  display.println(was);
  display.display();
}

void initOled()
{
 // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  delay(500);
  
}


//-------------------------------------------

 
 










 //------------------------------------------------------
