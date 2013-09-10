#ifndef Ethernet_h
#define Ethernet_h

#include <inttypes.h>
#include "EncClient.h"
#include "EncServer.h"
extern "C" {
    #include "utility/socket.h"
}

class EncEthernetClass {
private:
public:
  static uint8_t _state[MAX_SOCK_NUM];
  static uint16_t _server_port[MAX_SOCK_NUM];
  void begin(uint8_t *, uint8_t *);
  void begin(uint8_t *, uint8_t *, uint8_t *);
  void begin(uint8_t *, uint8_t *, uint8_t *, uint8_t *);
#ifdef ETHERSHIELD_DEBUG
  uint8_t *returnDebug();
  void clearDebug();
#endif
  friend class Client;
  friend class Server;
};

extern EncEthernetClass EncEthernet;

#ifdef ETHERSHIELD_DEBUG
char *debug2str(uint8_t debugCode);
#endif
#endif
