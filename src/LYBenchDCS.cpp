#include "Arduino.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

const byte outInh = LED_BUILTIN;
const byte outVsel = 12;
volatile byte inh; //start in INHIBIT 
volatile byte vsel; //start in V0


#include <rotronics.h>
#define HYGROCLIP_PIN 8
rotronics hygroClip(HYGROCLIP_PIN);

// Here we can implement a safety interlock to turn off HV as an interrupt
// const byte interruptInh = 2;
// volatile byte fitpixBusy1 = LOW;
// void busyON1() {
//   fitpixBusy1 = digitalRead(interruptInh);
// }

void readRotronics()
{
  uint8_t buf[7];
  hygroClip.read_measurement(buf);
  if (!rotronics::measurementCheckSum(buf))
      Serial.println("Measurement chksum not correct");
  else
    {
      Serial.print("T: ");
      Serial.print(rotronics::measuredTemp(buf));
      Serial.print(" H: ");
      Serial.println(rotronics::measuredHum(buf));
    }
}

void serialEvent() {    
  while (Serial.available()) { //here wait for busy OFF signal on serial
    // get the new byte:
    char inChar = (char)Serial.read();
    //    Serial.print(inChar);    
    if (inChar == 'o' || inChar == 'O') { // ON state
      inh = LOW;
      Serial.println("ON");
    }
    else if (inChar == 'i' || inChar == 'I') { //inhhit state
      inh = HIGH;
      Serial.println("INH");
    }
    else if (inChar == '1') { //V1 (VSEL=1)
      vsel = HIGH;
      Serial.println("V1");
    }
    else if (inChar == '0') { //V0
      vsel = LOW;
      Serial.println("V0");
    }
    else if (inChar == 't' || inChar == 'T') { //read temperature
       readRotronics();
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(outInh, OUTPUT);
  pinMode(outVsel, OUTPUT);
  //Start in inhbit mode and V0
  inh=TRUE; 
  vsel=LOW;
  digitalWrite(outInh, inh);
  digitalWrite(outVsel, vsel);
  // Safety interlock (ie comparator on photodiode to check of the box gets open)
  // pinMode(interruptInh, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(interruptInh), busyON1, CHANGE);
}

void loop() {
  digitalWrite(outInh, inh);
  digitalWrite(outVsel, vsel);
}

