#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <map>
#include "voteserver.h"
#include "tcptextchannel.h"
#include "tcpbroadcastchannel.h"

#define PORT 3344

int ssock;
std::vector<pollfd> poll_list;

TcpBroadcastChannel broadcastChannel(poll_list);
// std::map<int, TcpTextChannel*> channels;
std::map<int, TcpTextChannel> channels;
VoteServerModel server(&broadcastChannel);

void handle_new_connection()
{
    int csock, i;
    
    csock = accept(ssock, NULL, NULL);
    if(csock < 0) return;
    
    std::cout << "Opening connection: " << csock << std::endl;
    poll_list.push_back({
        .fd = csock,
        .events = POLLIN
    });
    // TcpTextChannel* newChannel = new TcpTextChannel{csock};
    // channels[csock] = newChannel;
    // server.registerUserChannel(newChannel);
    auto r = channels.emplace(std::piecewise_construct, std::forward_as_tuple(csock), std::forward_as_tuple(csock));
    server.registerUserChannel(&r.first->second);
}

void process_close(int i, int csock)
{
    std::cout << "Closing connection: " << poll_list[i].fd << std::endl;
    std::swap(poll_list[i], poll_list[poll_list.size() - 1]);
    poll_list.pop_back();
    close(csock);
    // delete channels[csock];
    channels.erase(csock);
}

void process_read(int csock)
{
    char buf[256];
    int len;
    
    len = recv(csock, buf, sizeof(buf), 0);
    if(len > 0)
    {
        // channels[csock]->receiveMessage(buf, len);
        channels.at(csock).receiveMessage(buf, len);
    }
}

int main() 
{
    struct sockaddr_in6 addr;
    int reuse;
    int i;
    
    if((ssock = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
    {
        perror("socket"); 
        return 1;
    }
    
    reuse = 1;
    setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(PORT);
    
    if(bind(ssock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");    
        return 1;
    }
    
    if(listen(ssock, 5) < 0)
    {
        perror("listen");
        return 1;
    }
    
    poll_list.push_back({ //pollfd
        .fd = ssock,
        .events = POLLIN
    });
    
    while(1)
    {
        if(poll(poll_list.data(), poll_list.size(), -1) > 0)
        {
            if(poll_list[0].revents & POLLIN)
            {
                handle_new_connection();
            }
          
            for(i = 1; i < poll_list.size(); i++)
            {
                if(poll_list[i].revents & (POLLERR | POLLHUP))
                {
                    process_close(i, poll_list[i].fd);
                }
                else if(poll_list[i].revents & POLLIN)
                {
                    process_read(poll_list[i].fd);
                }
            }
        }
    }
    
    return 0;
}
