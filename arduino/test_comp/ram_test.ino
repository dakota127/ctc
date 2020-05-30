//--------------------------------------------
//  Memory  Testmodule 1
//
//  The function ram_test() is called from the test_comp.ino module
//
// Nov. 2019  Peter K. Boxler
// -------------------------------------------


//---------------------------------------------
int ram_test_write() {
int addr;
int data_byte;


 Serial.println ("RAM Test...");
 writeLine (2, "Wrting to RAM...");
 Serial.println ("RAM write");
 
// write to RAM ---------------------------------
  for (addr = 0; addr <256; addr++) {
    data_byte = addr % 16;    
 //   Serial.print ("addr: "); Serial.println (addr);
 //   Serial.print ("data: "); Serial.println (data_byte, HEX);
    //delay(4000);
   // read the value from the adc on pin PIN_POT
    // this defines the speed
    potValue = analogRead(PIN_POT);
    delay_time = potValue * 4;  
  //  Serial.println (delay_time);
    if (ir_happened == true) {
      writeLine (4, "cancelled..");
      
      delay(50);
      delay(1000);;
      return(99);
    }
  
//  delay_time = 2000;
 //   Serial.print ("bus: ");  Serial.println (busValue);
   setBus(addr);              // shift output data to bus
   enableBus(true);        // enable output
   delay (delay_time);

//  set control lines
// to write to the ram we first need to write the desired adress to the memory address register. This is done
// by activating the MARW ctrl line. 
// if we need to activate more than one control line we would do it like this:
//  ctrl_out =  CTRL_LINE0 | CTRL_LINE5;
//
//  the board under test needs these ctrl lines: MARW, ME, MW 
//
// set Adress into MAR
   ctrl_out = CTRL_LINE0;     // set MARW write  (wird invertiert aucf ctc board)
 //  Serial.print ("ctrl_out: "); Serial.println(ctrl_out);
   setCtrl(ctrl_out);
   enableCtrl(true);        // enable the ctrl lines
   delay(delay_time);   
   pulseClock();            // pulse clock, Adresss is stored in MARW
   delay(delay_time);
   ctrl_out = 0;            // alle disabled
   setCtrl(ctrl_out);
   enableBus(false);        // set Ctrl off
   delay(delay_time);
   // assert data on the buss
   setBus(data_byte);              // shift output data to bus
   enableBus(true);        // enable output
   delay (delay_time);  
   ctrl_out = CTRL_LINE1;     // set MW Memory write
 //  Serial.print ("ctrl_out: "); Serial.println(ctrl_out);
   setCtrl(ctrl_out);
   enableCtrl(true);        // enable the ctrl lines  
   delay(delay_time);   
   pulseClock();            // pulse clock
   delay(delay_time);  
   ctrl_out = 0;            // alle disabled
   setCtrl(ctrl_out);
   delay(delay_time);  
  }

  return (0);
}

//---------------------------------------------
int ram_test_read() {
int addr;
int data_byte;
int error_anz = 0;   

  writeLine (2, "Reading from RAM...");
    Serial.println ("RAM read");
// read from to RAM ---------------------------------
  for (addr = 0; addr <256; addr++) {
    data_byte = addr % 16;    
//    Serial.print ("addr: "); Serial.println (addr);
//    Serial.print ("data: "); Serial.println (data_byte, HEX);


   // read the value from the adc on pin PIN_POT
    // this defines the speed
    potValue = analogRead(PIN_POT);
    delay_time = potValue * 4;  
 //   Serial.println (delay_time);
    if (ir_happened == true) {
      writeLine (4, "cancelled..");
      
      delay(50);
      delay(1000);;
      return(99);
    }
    
 //   Serial.print ("bus: ");  Serial.println (busValue);
   setBus(addr);              // shift output data to bus
   enableBus(true);        // enable output
   delay (delay_time);

//
//  the board under test needs these ctrl lines: MARW, ME, MW 
//
// set Adress into MAR
   ctrl_out = CTRL_LINE0;     // set MARW write
 //  Serial.print ("ctrl_out: "); Serial.println(ctrl_out);
   setCtrl(ctrl_out);
   enableCtrl(true);        // enable the ctrl lines
   delay(delay_time);   
   pulseClock();            // pulse clock, Adresss is stored in MARW
   delay(delay_time);
   ctrl_out = 0;            // alle disabled
   setCtrl(ctrl_out);
   enableBus(false);        // set Ctrl off
   delay(delay_time);
   
   // enable ME 
 
   ctrl_out = CTRL_LINE2;     // set ME Memory enable
 //  Serial.print ("ctrl_out: "); Serial.println(ctrl_out);
   setCtrl(ctrl_out);
   enableCtrl(true);        // enable the ctrl lines  
   delay(delay_time);   

  int a=readBus();        // read what is now on the bus

  if (a !=  addr % 16) {
     error_anz +=1;
    Serial.print("--------> error bei adr: "); Serial.print (addr); Serial.print (" / "); Serial.print (addr%16); Serial.print (" / "); Serial.println (a); 
  }
   delay(delay_time);  
   ctrl_out = 0;            // alle disabled
   setCtrl(ctrl_out);
   enableCtrl(false);       // set bus off 
   delay(delay_time);  
  }

  return (error_anz);
}
//-----------------------------------------------
void ram_test() {

int error; 

    error = ram_test_write();
    error = ram_test_read();

    if (error == 99) {
      Serial.println (cancel);
      return(99);
    }
   Serial.print ("Anzahl Errors RAM: "); Serial.println (error);
    if (error == 0)  writeLine (3, "Ram ok");
    else writeLine (3, "Ram not ok");
 
    
   delay(1000);
   

  
}
//----------------------------------

//----------------------------------
