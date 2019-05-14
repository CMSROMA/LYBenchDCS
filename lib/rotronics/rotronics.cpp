/*
rotronics by Paolo Meridiani
*/

#include <Arduino.h>
#include <rotronics.h>

void rotronics::begin(uint8_t pin)
{
  pinMode(pin, INPUT);
  myPin=pin;
}

//
// Read a bit. 
//
uint8_t rotronics::read_bit(void)
{
  uint8_t r;
  // loop until a 0 is not found
  // timings from rotronics datasheet:
  // 1 has a maximum time of 115us
  // 0 has a minimum time of 240us
  // value read at ~180us should return the value of the bit 
  // bit length typical 470us, maximum 540us
  r=1;
  
  //try to catch the transition to 0
  while (r!=0)
    r = digitalRead(myPin);
	
  noInterrupts();
  delayMicroseconds(180);
  r = digitalRead(myPin);
  interrupts();
  delayMicroseconds(180);
  return r;
}


void rotronics::read_measurement(uint8_t *buf) 
{
  //catch the pause between measurements (at least 10ms at logic 1)
  int cnt=0;
  while (cnt<1000)
    {
      delayMicroseconds(10);
      uint8_t val=digitalRead(myPin);
      if (val==0)
	cnt=0;
      else
	cnt++;
    }

  uint8_t val=read_bit();
  if (val!=0)
    {
      for(int i=0;i<7;++i)
	buf[i]=0xFF;
      return;
    }

  //this is important since the first 0 happens to be longer
  delayMicroseconds(1000);

  //read 56 bits 
  bool bits[56];
  for (int ibit=0;ibit<56;++ibit)
      bits[ibit]=rotronics::read_bit();

  //now convert into bytes (LSB goes first for each byte)
  uint8_t bitMask;
  for (int ibit=0;ibit<56;++ibit) 
    {
      if (ibit%8==0)
	buf[ibit/8]=0;
      bitMask=1<<(ibit%8);
      if (bits[ibit])
	buf[ibit/8] |= bitMask;
    }
}

bool rotronics::measurementCheckSum(uint8_t *buf) 
{
  uint8_t cksum=0;
  for (uint8_t i=0;i<6;++i)
      cksum+=buf[i];
  if ( cksum != buf[6] )
    return false;
  return true;
}

float rotronics::measuredTemp(uint8_t *buf) 
{
  if (buf[0] != 0x54)
      return 999.;

  float temp = float(buf[1])/256. + buf[2] - 50.;
  return temp;
}

float rotronics::measuredHum(uint8_t *buf) 
{
  if (buf[3] != 0x46)
    return 999.;

  float temp = float(buf[4])/256. + buf[5] ;
  return temp;
}
