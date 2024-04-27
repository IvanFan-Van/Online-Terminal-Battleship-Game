#ifndef CTCPCLIENT_H
#define CTCPCLIENT_H

#include <cstring>
#include <iostream>
#include <string>

class CTcpClient {
private:
  int socket_fd;
  std::string server_ip;
  unsigned short server_port;

public:
  CTcpClient();
  ~CTcpClient();

  bool connect(const std::string &server_ip, const int server_port);
  bool send(const std::string &buffer);
  bool send(const void *buffer, const size_t len);
  bool recv(std::string &buffer, const size_t maxlen);
  bool recv(void *buffer, const size_t len);
  bool close();

  int getSocketFd() const;
};

#endif // CTCPCLIENT_H