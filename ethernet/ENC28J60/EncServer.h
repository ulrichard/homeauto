#ifndef Server_h
#define Server_h

#include <Print.h>

class EncClient;

class EncServer : public Print {
private:
  uint16_t _port;
  void accept();
public:
  EncServer(uint16_t);
  EncClient available();
  void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const char *str);
  virtual size_t write(const uint8_t *buf, size_t size);
};

#endif
