#pragma once

#ifdef _WIN32

#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define close closesocket
#define chdir _chdir
#define getcwd _getcwd

#else

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif