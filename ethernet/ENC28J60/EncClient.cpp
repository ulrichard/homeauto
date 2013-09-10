extern "C" {
  #include "utility/socket.h"
  #include "string.h"
}

#include <Arduino.h>

#include "EncEthernet.h"
#include "EncClient.h"
#include "EncServer.h"

uint16_t EncClient::_srcport = 0;

EncClient::EncClient(uint8_t sock) {
  _sock = sock;
}

EncClient::EncClient(uint8_t *ip, uint16_t port) {
  _ip = ip;
  _port = port;
  _sock = 255;
}

uint8_t EncClient::connect() {
  if (_sock != 255)
    return 0;
#ifdef ETHERSHIELD_DEBUG
    sprintf(_DEBUG, "EncClient::connect() DEBUG A. _sock = %d",
            _sock);
#endif

  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    uint8_t s = getSn_SR(i);
    if (s == SOCK_CLOSED || s == SOCK_FIN_WAIT) {
      _sock = i;
      break;
    }
  }

#ifdef ETHERSHIELD_DEBUG
    sprintf(_DEBUG, "EncClient::connect() DEBUG B. _sock = %d",
            _sock);
#endif
  if (_sock == 255)
    return 0;

  _srcport++;
  if (_srcport + 1024 == 0) _srcport = 0;
  socket(_sock, Sn_MR_TCP, _srcport + 1024, 0);

#ifdef ETHERSHIELD_DEBUG
    sprintf(_DEBUG, "Client::connect() DEBUG C. _sock = %d",
            _sock);
#endif
  if (!::connect(_sock, _ip, _port)) {
    _sock = 255;
    return 0;
  }

#ifdef ETHERSHIELD_DEBUG
    sprintf(_DEBUG, "Client::connect() DEBUG D. _sock = %d", _sock);
#endif
  while (status() != SOCK_ESTABLISHED) {
    delay(1);
    if (status() == SOCK_CLOSED) {
      _sock = 255;
      return 0;
    }
  }

#ifdef ETHERSHIELD_DEBUG
    sprintf(_DEBUG, "EncClient::connect() DEBUG E. _sock = %d", _sock);
#endif
  return 1;
}

#ifdef ETHERSHIELD_DEBUG
char *EncClient::debug() {
    return _DEBUG;
}
#endif

size_t EncClient::write(uint8_t b) {
  if(_sock != 255)
  {
    send(_sock, &b, 1);
    return 1;
  }
  return 0;
}

size_t EncClient::write(const char *str) {
  if (_sock != 255)
  {
    send(_sock, (const uint8_t *)str, strlen(str));
    return strlen(str);
  }
  return 0;
}

size_t EncClient::write(const uint8_t *buf, size_t size) {
  if (_sock != 255)
  {
    send(_sock, buf, size);
    return size;
  }
  return 0;
}

int EncClient::available() {
  if (_sock != 255)
    return getSn_RX_RSR(_sock);
  return 0;
}

int EncClient::read() {
  uint8_t b;
  if (!available())
    return -1;
  recv(_sock, &b, 1);
  return b;
}

void EncClient::flush() {
  while (available())
    read();
}

void EncClient::stop() {
  if (_sock == 255)
    return;

  // attempt to close the connection gracefully (send a FIN to other side)
  disconnect(_sock);
  unsigned long start = millis();

  // wait a second for the connection to close
  while (status() != SOCK_CLOSED && millis() - start < 1000)
    delay(1);

  // if it hasn't closed, close it forcefully
  if (status() != SOCK_CLOSED)
    close(_sock);

  EncEthernetClass::_server_port[_sock] = 0;
  _sock = 255;
}

uint8_t EncClient::connected() {
  if (_sock == 255) {
    return 0;
  } else {
    uint8_t s = status();
    return !(s == SOCK_LISTEN || s == SOCK_CLOSED || s == SOCK_FIN_WAIT ||
      (s == SOCK_CLOSE_WAIT && !available()));
  }
}

uint8_t EncClient::status() {
  if (_sock == 255) {
    return SOCK_CLOSED;
  } else {
    return getSn_SR(_sock);
  }
}

// the next three functions are a hack so we can compare the client returned
// by Server::available() to null, or use it as the condition in an
// if-statement.  this lets us stay compatible with the Processing network
// library.

uint8_t EncClient::operator==(int p) {
  return _sock == 255;
}

uint8_t EncClient::operator!=(int p) {
  return _sock != 255;
}

EncClient::operator bool() {
  return _sock != 255;
}
