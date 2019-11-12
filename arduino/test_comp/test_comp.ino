/*
 * Arduino Sketch Unit Test Components of 8-bit computer
 * 
 * This is the main module, the other modules are used to test individual components
 * There is one module for:
 *  General purpose registers, two on a board
 *  Memory Module
 *  ALU Module
 *  etc...

   Nov. 2019  Peter K. Boxler
 * 
 */
#include "Arduino.h"
#include <stdint.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);


#define DEBUGLEVEL 1        // für Debug Output, für Produktion DEBUGLEVEL 0 setzen !
//#define TESTOUT           // Testoutput at setup time
#include <DebugUtils.h>     // Library von Adreas Spiess

// bus Shift out
#define PIN_DATA_OUT 2      // to Pin 14 on HC595
#define PIN_BUS_CLK 10      // to Pin 11 on HC595
#define PIN_BUS_LATCH 4     // to Pin 12 on HC595

// Control shift out
#define PIN_CTRL_OUT 2      // to Pin 14 on HC595
#define PIN_CTRL_CLK 5      // to Pin 11 on HC595
#define PIN_CTRL_LATCH 6    // to Pin 12 on HC595

#define PIN_BUS_ENABLE 7    // to Pin 13 on HC595
#define PIN_CTRL_ENABLE 8   // to Pin 13 on HC595

#define PIN_CLOCK 9         // Clock out

#define PIN_BUS_SELECT  13  // Bus read select pin
#define PIN_BUS_BIT_0_4 A1  // bus read
#define PIN_BUS_BIT_1_5 A0  // bus read
#define PIN_BUS_BIT_2_6 A3  // bus read
#define PIN_BUS_BIT_3_7 A2  // bus read
#define BUTTON_GO    3      // Interrrupt 1

#define PIN_POT A7          // adc in for potentiometer (speed)
#define MAX_ANZ_component 5      // number of implemented components
#define PIN_SEL_FUNC A6     // Input adc, used to select componenttion to be run


#define DEBOUNCE 30       // milliseconds

// control lines 8 defined so far
#define CTRL_LINE0 (1 << 0)  // control lines
#define CTRL_LINE1 (1 << 1)
#define CTRL_LINE2 (1 << 2)
#define CTRL_LINE3 (1 << 3)
#define CTRL_LINE4 (1 << 4)
#define CTRL_LINE5 (1 << 5)
#define CTRL_LINE6 (1 << 6)
#define CTRL_LINE7 (1 << 7)
#define CTRL_LINE8 (1 << 8)  // control lines
#define CTRL_LINE9 (1 << 9)
#define CTRL_LINE10 (1 << 10)
#define CTRL_LINE11 (1 << 11)
#define CTRL_LINE12 (1 << 12)
#define CTRL_LINE13 (1 << 13)
#define CTRL_LINE14 (1 << 14)
#define CTRL_LINE15 (1 << 15)

bool busBits [8];       // for reading from the bus
unsigned long old_time_component=0;        // to debounce componenttion pusbutton
volatile unsigned long old_time_go=0; // to debounce go/cancel pusbutton
volatile bool ir_happened = false;
boolean funcButtonPressed = false; 
boolean goButtonPressed = false;
int potValue = 0;           // adc value for speed
int delay_time = 100;       //default
uint16_t ctrl_out;          // control lines out   
int selValue = 0;
int component = 0;
int mode = 0;           // mode the sketch is in: either SELFUNC or RUNNING


enum {
    RUNNING,                                      // finite state machins states
    SELFUNC,

    };     

int linepos[] = {0,0,22,40,52};           // y-position on OLED (see Adafruit GFX Library)
                                          // line 1: Titel
                                          // line 2: component to be selected or running
                                          // line3 and 4: display result   
// strings to put to lcd/oled display

static const char  *mycomponents2[] = {"Select Test: Reg 1","Select Test: Reg 2", 
                                        "Select Test: Ram", "Select Test: Alu   ", "Select Test: PC", "Select Test: IR"
                    };                  

/*
//-------------------------------------------
void test_output() {

#ifdef TESTOUT

    enableBus(true);
    enableCtrl(true);  
    setBus(0);
    setCtrl(0);

    Serial.println("xff und CTRL_RA_E");
    delay(2000);
    setBus(0xff);
    delay(20);
    setCtrl(0x0);
    setCtrl(CTRL_RA_E);
    delay(2000);

    Serial.println("x12 und CTRL_RA_W");
    setBus(0x12);
    delay(20);
    setCtrl(0x0);
    setCtrl(CTRL_RA_W);
    delay(2000);

    Serial.println("A3 und CTRL_RB_E");
    setBus(0xA3);
    delay(20) ; 
    setCtrl(0x0);
    setCtrl(CTRL_RB_E);
    delay(2000);

    Serial.println("xf4 und CTRL_RB_W");
    setBus(0xf4);
    delay(20)  ; 
    setCtrl(0x0);
    setCtrl(CTRL_RB_W);
    delay(2000);
    setCtrl(0x0);
  
    pulseClock();

#endif  
}
*/

//-------------------------------------------
void enableBus(bool what){
  
  digitalWrite (PIN_BUS_ENABLE, what ? LOW :HIGH);
  
}

//-------------------------------------------
void enableCtrl(bool what){
  
  digitalWrite (PIN_CTRL_ENABLE, what ? LOW : HIGH);
  
}

//-------------------------------------------
void setBus(int data_out){
  shiftOut (PIN_DATA_OUT, PIN_BUS_CLK, LSBFIRST, data_out);
  digitalWrite (PIN_BUS_LATCH, LOW);
  digitalWrite (PIN_BUS_LATCH, HIGH);
  digitalWrite (PIN_BUS_LATCH, LOW);
  
}

//-------------------------------------------
void pulseClock(){

  digitalWrite (PIN_CLOCK, HIGH);
  delay(100);
  digitalWrite (PIN_CLOCK, LOW);
  
}

//-------------------------------------------
void setCtrl(uint16_t ctrl_bits){
  shiftOut (PIN_DATA_OUT, PIN_CTRL_CLK, MSBFIRST, ctrl_bits << 8);
  shiftOut (PIN_DATA_OUT, PIN_CTRL_CLK, MSBFIRST, ctrl_bits);
  digitalWrite (PIN_CTRL_LATCH, LOW);
  digitalWrite (PIN_CTRL_LATCH, HIGH);
  digitalWrite (PIN_CTRL_LATCH, LOW);
  
}

//---------------------------------------------
int readBus() {       //  to read 8 bits from the bus

  int from_Bus = 0;

  for (int y=0; y<8;y++) {
    busBits[y]=0;
  }

 // Bus signal read via Data Selector 74NCT157
 // only 5 pins are needed to read 8 bits from bus
  digitalWrite (PIN_BUS_SELECT, HIGH);            // Select B Inputs 74NCT157
  busBits[4] = digitalRead (PIN_BUS_BIT_0_4);     // read bit 4
  busBits[5] = digitalRead (PIN_BUS_BIT_1_5);     // read bit 3
  busBits[6] = digitalRead (PIN_BUS_BIT_2_6);     // read bit 2
  busBits[7] = digitalRead (PIN_BUS_BIT_3_7);     // read bit 1  

 digitalWrite (PIN_BUS_SELECT, LOW);                // Select A Inputs 74NCT157
  busBits[0] = digitalRead (PIN_BUS_BIT_0_4);     // read bit 0
  busBits[1] = digitalRead (PIN_BUS_BIT_1_5);     // read bit 1
  busBits[2] = digitalRead (PIN_BUS_BIT_2_6);     // read bit 2
  busBits[3] = digitalRead (PIN_BUS_BIT_3_7);     // read bit 3  
  
  
  for (int i=8; i>0; i--) {
  //  Serial.print (busBits[i-1]); 
    from_Bus = from_Bus | (busBits[i-1] << (i-1));    
  }
 
 //  Serial.println(from_Bus,HEX);
   return(from_Bus);
 
}

// Interrrupt routine go/cancel button on Pin 3
//-------------------------------------------
void ir_GO_CANCEL() {
  
  if((millis() - old_time_go) > DEBOUNCE) { 
    // innerhalb der DEBOUNCE nichts machen
 
    old_time_go = millis(); // letzte Schaltzeit merken  
    ir_happened = true;
  }
}

//  to select which component to test
//  each buttonpress (button 1) changes display 
//  interrupt (button 2) selects the component
// -------------------------------------------
int whatcomponent() {

bool done = false;

  int component_sel = 0;        // selected component

 // Serial.println ("whatcomponent()");
  writeLine (2, mycomponents2[component_sel]);    // write comp to oled
  
  do {
   
      selValue = analogRead(PIN_SEL_FUNC);
    //DEBUGPRINTLN1 (selValue);
  
      if (selValue < 20 && funcButtonPressed == false) {   // Wenn der Taster aktuell gedrÃ¼ckt wird, ...
        funcButtonPressed = true;                                 // merke dir, dass der Taster gedrÃ¼ckt wurde, 
        old_time_component = millis();         // letzte Schaltzeit merken 
        }
      if (selValue >600 && funcButtonPressed == true) {
        if((millis() - old_time_component) > DEBOUNCE) {  
 
       //   Serial.print("component = ");
       //   Serial.println (component_sel);
          component_sel+=1;
          writeLine (2, mycomponents2[component_sel]);
          delay(30); 
        funcButtonPressed = false; 
        Serial.println(component_sel);
      }
      
    }
    
    if (component_sel > MAX_ANZ_component) {
      component_sel = 0;   //wrap around
      writeLine (2, mycomponents2[component_sel]);      // write to oled on line 2
      delay(30); 
    }
  }
   while (ir_happened == false);            // loop until IR happened (from button 2)

  writeLine (3, "   ");                     // clear result lines on display
  writeLine (4, "   ");
  return (component_sel+1);                 // return selected comp
   
}

//-------------------------------------------
//-------------------------------------------
// the setup componenttion runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  delay(1000);  // This delay is needed to let the display to initialize

  Serial.println("About to test the tester...");
 
  pinMode(PIN_DATA_OUT, OUTPUT);
  
  pinMode(PIN_BUS_CLK, OUTPUT);
  pinMode(PIN_BUS_LATCH, OUTPUT);
  pinMode(PIN_BUS_ENABLE, OUTPUT);
  
  pinMode(PIN_CTRL_CLK, OUTPUT);
  pinMode(PIN_CTRL_LATCH, OUTPUT);
  pinMode(PIN_CTRL_ENABLE, OUTPUT);  

  pinMode(PIN_CLOCK, OUTPUT);  

  pinMode(PIN_BUS_BIT_0_4, INPUT);  
  pinMode(PIN_BUS_BIT_1_5, INPUT);  
  pinMode(PIN_BUS_BIT_2_6, INPUT);  
  pinMode(PIN_BUS_BIT_3_7, INPUT);  
  pinMode(PIN_BUS_SELECT, OUTPUT);  
 
  digitalWrite (BUTTON_GO, HIGH);   // interner Pull up Widerstand auf 5V
 
  digitalWrite (PIN_BUS_LATCH, LOW);
  digitalWrite (PIN_CTRL_LATCH, LOW);
  digitalWrite (PIN_BUS_ENABLE, HIGH);     // disable
  digitalWrite (PIN_CTRL_ENABLE, HIGH);    // disable
  digitalWrite (PIN_CLOCK, LOW);          // disable
  digitalWrite (PIN_BUS_SELECT, LOW);

  mode = SELFUNC;
    
  attachInterrupt(digitalPinToInterrupt(BUTTON_GO), ir_GO_CANCEL, LOW);   

  initOled();         
  writeTitle();
  display.setFont();    // reset to regular font
  Serial.println ("Setup done");

}


//-------------------------------------------
//-------------------------------------------
void loop() {

/*
  // read the value from the sensor:
  potValue = analogRead(PIN_POT);
  // Serial.println(potValue);
  delay_time = potValue * 3;
 */ 
  component = 1;          // default      
  ir_happened == false;
  mode = SELFUNC;
  component = whatcomponent();
  DEBUGPRINTLN1 ("component ok");

  mode = RUNNING;
  ir_happened = false;

//  check the component that was selected
  switch (component) {
  case 1:
    reg_board1();
    break;

  case 2:
    reg_board2();
    break;
    
 case 3:
    mem_test();
    delay(100);;
    break;
    
  case 4:
    alu_test();
    delay(100);;
    break;

  case 5:   
    pc_test();
    delay(100);;
    break;

  case 6:   
    ir_test();
    delay(100);;
    break;
       
   default:
   Serial.println ("wrong component");
   break; 
  }
  

 ir_happened = false;
 
}

//End of sketch
//-------------------------------------------
