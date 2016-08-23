#ifndef VOTESERVER_H
#define VOTESERVER_H

#include "voteobjects.h"
#include "serialization.h"
#include "itextchannel.h"

class VoteServerModel {

    struct VotingModel {
        Voting voting;
        std::map<int, int> votes;
    };

    /*Communication channel to every user*/
    ITextChannel* broadcastChannel;
    /*Server state*/
    std::map<int, VotingModel> votings;
    int currentUserId = 0;
    int currentVotingId = 0;
public:
    VoteServerModel(ITextChannel* channel): broadcastChannel{channel} {
        loadTestData();
    }
    void registerUserChannel(ITextChannel* userChannel) {
        int userId = -1;
        userChannel->messageReceived = [this, userChannel, userId](std::string msg) mutable {
            onMessageReceive(msg, userId, userChannel);
        };
    }
private:
    void onMessageReceive(std::string& messageText, int& userId, ITextChannel* userChannel) {
        std::istringstream is(messageText);
        int command;
        is >> command;
        switch (command) {
            case Login:
                userId = ser::parseMessage<int>(messageText);
                loginOrRegister(LoginOk, userId, userChannel);
                break;
            case Register:
                userId = currentUserId++;
                loginOrRegister(RegisterOk, userId, userChannel);
                break;
            case CreateVoting:
                createVoting(ser::parseMessage<Voting>(messageText));
                break;
            case DeleteVoting:
                deleteVoting(ser::parseMessage<int>(messageText));
                break;
            case VoteFor:
                voteFor(userId, ser::parseMessage<Vote>(messageText));
                break;
            default: throw std::bad_function_call{}; //"Invalid message command."
        }
    }

    void loginOrRegister(int command, int userId, ITextChannel* userChannel) {
        Votings voting;
        voting.myId = userId;
        for (auto& entry : votings) {
            voting.votings.push_back(entry.second.voting);
            auto voteIt = entry.second.votes.find(userId);
            if (voteIt != entry.second.votes.end()) {
                Vote newVote;
                newVote.votingId = entry.first;
                newVote.option = voteIt->second;
                voting.myVotes.push_back(newVote);
            }
        }
        userChannel->sendMessage(ser::composeMessage(command, voting));
    }

    void createVoting(const Voting& voting) {
        int newId = currentVotingId++;
        votings[newId] = VotingModel{ voting };
        votings[newId].voting.id = newId;
        broadcastChannel->sendMessage(ser::composeMessage(VotingCreated, votings[newId].voting));
    }

    void deleteVoting(int votingId) {
        votings.erase(votingId);
        broadcastChannel->sendMessage(ser::composeMessage(VotingDeleted, votingId));
    }

    void voteFor(int userId, const Vote& vote) {
        int prevVote = -1, newVote = vote.option;
        auto& model = votings[vote.votingId];
        if (model.votes.find(userId) != model.votes.end())
            prevVote = model.votes[userId];

        model.votes[userId] = newVote;
        if (prevVote >= 0)
            model.voting.results[prevVote]--;
        model.voting.results[newVote]++;

        broadcastChannel->sendMessage(ser::composeMessage(VotingChanged, VotingUpdate{ vote.votingId,  model.voting.results }));
    }

    void loadTestData() {
        Voting test;
        test.title = "Hanyast er ez a nagyhazi?";
        test.description = "Kerem toltse ki az ertekeleset!";
        test.owner = "Gyuris Bence (MFSX9D)";
        test.options = { "5", "4", "3", "2", "1" };
        test.results = { 0, 0, 0, 0, 0 };
        test.expiration = 0;
        createVoting(test);

        Vote vote;
        vote.option = 0;
        vote.votingId = 0;
        voteFor(100, vote);
    }
};

#endif // VOTESERVER_H
