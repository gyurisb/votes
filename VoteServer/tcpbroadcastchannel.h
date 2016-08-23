#ifndef TCPBROADCASTCHANNEL_H
#define TCPBROADCASTCHANNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include "itextchannel.h"

class TcpBroadcastChannel : public ITextChannel {
    std::vector<pollfd>& poll_list;
public:
    TcpBroadcastChannel(std::vector<pollfd>& poll_list): poll_list(poll_list) {}
    virtual void sendMessage(std::string data) {
        for(int i = 1; i < poll_list.size(); i++) {
            std::cout << " =>" << poll_list[i].fd << ": " << data << std::endl;
            send(poll_list[i].fd, data.c_str(), data.size(), 0);
            send(poll_list[i].fd, "\n", 1, 0);
        }
    }
};

#endif