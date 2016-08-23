#ifndef VOTEOBJECTS_H
#define VOTEOBJECTS_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <forward_list>
#include <stdexcept>
#include <fstream>

struct Vote {
    int votingId;
    int option;
};

struct VotingUpdate {
    int id;
    std::vector<int> results;
};

struct Voting {
    int id;
    std::string owner;
    std::string title;
    std::string description;
    std::string imageUrl;
    std::vector<std::string> options;
    int expiration;
    std::vector<int> results;
};

struct Votings {
    int myId;
    std::vector<Voting> votings;
    std::vector<Vote> myVotes;
};

enum UserCommands {
    Login = 0,
    Register = 1,
    CreateVoting = 2,
    DeleteVoting = 3,
    VoteFor = 4
};

enum ServerCommands {
    VotingChanged = 5,
    VotingCreated = 6,
    VotingDeleted = 7,
    LoginOk = 8,
    RegisterOk = 9,
    MyVote = 10
};

#endif // VOTEOBJECTS_H
