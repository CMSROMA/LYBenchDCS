#ifndef rotronics_h
#define rotronics_h

#ifdef __cplusplus
#include <stdint.h>
#include <Arduino.h>       // for delayMicroseconds

class rotronics
{
  private:
  uint8_t myPin;

  // Read a bit.
  uint8_t read_bit(void);

  public:
    rotronics(uint8_t pin) { begin(pin); }
    void begin(uint8_t pin);

    // Read a T & H measurement (7 bytes buffer should be provided) 
    void read_measurement(uint8_t *buf);

    // Check buffer CheckSum
    static bool measurementCheckSum(uint8_t *buf);

    // Extract temperature from measured buffer
    static float measuredTemp(uint8_t *buf);

    // Extract humidity from measured buffer
    static float measuredHum(uint8_t *buf);
};

#endif // __cplusplus
#endif // rotronics_h
