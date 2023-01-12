#ifndef UDP_BROADCAST_H
#define UDP_BROADCAST_H

#pragma once

#include <iostream>
#include <functional>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

using namespace std;

#define BUFFER_SIZE 1024

extern bool exitProgram;

using onReadCB = std::function<void(const char* buffer, int buffer_len, struct sockaddr *addr, int addr_len)>;

class udp_interface {
public:
    udp_interface();
    ~udp_interface();

    void setOnReceive(onReadCB cb);

    void setTimeout(int seconds);

    onReadCB _on_read;

    bool is_timeout{false};

    struct timeval timeout{3,0};

};

class udp_broadcast : public udp_interface
{
public:
    udp_broadcast();
    ~udp_broadcast();

    bool send_broadcast(const char* smsg, int smsg_len, int port);
    bool receive_broadcast(int port);
};

class udp_multicast : public udp_interface
{
public:
    udp_multicast();
    ~udp_multicast();

    bool send_multicast(const char* smsg, int smsg_len, const char *group, int port);
    bool receive_multicast(const char *group, int port);
};

#endif