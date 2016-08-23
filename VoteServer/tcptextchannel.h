#ifndef TCPTEXTCHANNEL_H
#define TCPTEXTCHANNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include "itextchannel.h"

class TcpTextChannel : public ITextChannel {
    int csock;
    std::ostringstream stemp;
public:
    TcpTextChannel() = delete;
    TcpTextChannel(const TcpTextChannel&) = delete;
    TcpTextChannel& operator=(TcpTextChannel&) = delete;
    explicit TcpTextChannel(int csock): csock(csock) {}
    virtual void sendMessage(std::string data) {
        std::cout << " =>" << csock << ": " << data << std::endl;
        send(csock, data.c_str(), data.size(), 0);
        send(csock, "\n", 1, 0);
    }
    void receiveMessage(const char* msgPart, int len) {
        for (int i = 0; msgPart[i] && i < len; i++) {
            if (msgPart[i] == '\n') {
                std::string msg = stemp.str();
                stemp.str("");
                std::cout << csock << " =>: " << msg << std::endl;
                messageReceived(msg);
            }
            else {
                stemp << msgPart[i];
            } 
        }
    }
};

#endif