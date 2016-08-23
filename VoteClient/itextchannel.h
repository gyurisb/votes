#ifndef ITEXTCHANNEL_H
#define ITEXTCHANNEL_H

#include <string>
#include <functional>
#include <iostream>
#include <vector>

class ITextChannel {
public:
    virtual void sendMessage(std::string data) = 0;
    std::function<void(std::string)> messageReceived = [](std::string){};
};

#endif // ITEXTCHANNEL_H
