//--------------------------------------------
//  Register Board 1 Testmodule
//
//  The function reg_board1 () is called from the test_comp.ino module
//
//  This code tests 2 general purpose registers on one breadboard
//  Values from 0 to 255 are written to the bus and control signals are given
//  to write each value into the reg. Then the register is enabled (to give its contents) 
//  and the value is read from the bus and compared to what was written.
//  Nov. 2019  Peter K. Boxler
// -------------------------------------------
int reg_test1(int reg) {

int error_anz = 0;   

int busValue;
    writeLine (2, "Running...");
    delay(50);
  
  for (busValue = 0; busValue <256; busValue++) {          //  loop thru values from 0 to 255
    // read the value from the adc on pin PIN_POT
    // this defines the speed
    potValue = analogRead(PIN_POT);
    delay_time = potValue * 4;  
    
    if (ir_happened == true) {
      writeLine (4, "Cancelled...");
      
      delay(50);
      delay(1000);;
      return(99);
    }

// start/continue with the test
   setBus(busValue);              // shift output data to bus
   enableBus(true);        // enable output
   delay (delay_time);

//  set control lines
// to write to the reg we only need to activate one single control line (RxW).
// if we need to activate more than one control line we would do it like this:
//  ctrl_out =  CTRL_LINE0 | CTRL_LINE5;
//
//  the board under test has 2 GP registers
//  register 1 needs CTRL_LINE0
//  register 2 needs CTRL_LINE2
//
   ctrl_out = (1 ?  CTRL_LINE0: CTRL_LINE2);     // set Register write
   setCtrl(ctrl_out);
   enableCtrl(true);        // enable the ctrl lines
   delay(delay_time);
   
   pulseClock();            // pulse clock
   delay(delay_time > 400 ? 500 :0);
   
   enableCtrl(false);       // set bus off 
   enableBus(false);        // set Ctrl off
   delay(delay_time);
   
   ctrl_out = (1 ?  CTRL_LINE1: CTRL_LINE3);     // set Register enable
   setCtrl(ctrl_out);       // enable the ctrl lines
   enableCtrl(true);        

  int a=readBus();        // read what is now on the bus

  if (a != busValue){
     error_anz +=1;
  }

  String hstring =  String(a, HEX);
  hstring.toUpperCase();
  delay(delay_time);
  enableCtrl(false);      // disable control lines

  delay(delay_time);
}

 return (error_anz);

}

//-----------------------------------------------
void reg_board1() {

int error[] = {0,0};
    error[0] = reg_test1(1);

   
    if (error[0] == 99) {
      Serial.println ("test cancelled");
      return(99);
    }
    
   
    
    error[1] = reg_test1 (2);

    if (error[1] == 99) {
      Serial.println ("test cancelled");
      return(99);
    }

    if (error[0] == 0)  writeLine (3, "Reg 1 ok");
    else writeLine (3, "Reg 1 not ok");

    if (error[1] == 0)  writeLine (4, "Reg 2 ok");
    else writeLine (4, "Reg 2 not ok");
  
 
    
   delay(1000);
   

  
}
//----------------------------------
