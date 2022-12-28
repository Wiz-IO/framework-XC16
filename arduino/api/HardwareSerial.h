#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>
#include <Stream.h>

class HardwareSerial : public Stream
{
public:
  HardwareSerial(){};
  virtual ~HardwareSerial(){};
  virtual void begin(unsigned long) = 0;
  virtual void end() = 0;
  virtual int available() = 0;
  virtual int peek() = 0;
  virtual int read() = 0;
  virtual void flush() = 0;
  virtual size_t write(uint8_t) = 0;
  using Print::write;
  virtual operator bool() = 0;
};

extern void __attribute__((__weak__)) serialEventRun(void);

#endif // HardwareSerial_h
