#ifndef UDP_BROADCAST_H
#define UDP_BROADCAST_H

#pragma once

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

using namespace std;

extern bool exitProgram;

class udp_broadcast
{
public:
    udp_broadcast();
    ~udp_broadcast();
    bool send_broadcast(int port);
    bool receive_broadcast(int port);

private:

};

class udp_multicast
{
public:
    udp_multicast();
    ~udp_multicast();

    bool send_multicast(const char *group, int port);
    bool receive_multicast(const char *group, int port);

private:

};

#endif