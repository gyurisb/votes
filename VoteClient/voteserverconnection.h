#ifndef VOTESERVERCONNECTION_H
#define VOTESERVERCONNECTION_H

#include "voteclientmodel.h"
#include "serialization.h"
#include "itextchannel.h"

class VoteServerConnection final {
    VoteClientModel* clientModel;
    ITextChannel* serverChannel;
public:
    std::function<void(void)> connectionCreated = [](){};

    VoteServerConnection(VoteClientModel* clientModel, ITextChannel* serverChannel): serverChannel{serverChannel}, clientModel{clientModel} {
        serverChannel->messageReceived = [this](std::string msg) { onMessageReceive(msg); };
    }
    VoteServerConnection(const VoteServerConnection&) = delete;

    void startConnection() {
        if (clientModel->registered()) {
            serverChannel->sendMessage(ser::composeMessage(Login, clientModel->userId()));
        } else {
            serverChannel->sendMessage(ser::composeMessage(Register, 0));
        }
    }
    void createVoting(const Voting& voting) {
        serverChannel->sendMessage(ser::composeMessage(CreateVoting, voting));
    }
    void deleteVoting(int votingId) {
        serverChannel->sendMessage(ser::composeMessage(DeleteVoting, votingId));
    }
    void vote(const Vote& vote) {
        clientModel->setMyVote(vote);
        serverChannel->sendMessage(ser::composeMessage(VoteFor, vote));
    }

    VoteClientModel* voteClientModel() { return clientModel; }
private:
    void onMessageReceive(std::string& messageText) {
        std::istringstream is(messageText);
        int command;
        is >> command;
        switch (command) {
            case VotingChanged:
                clientModel->updateVoting(ser::parseMessage<VotingUpdate>(messageText));
                break;
            case VotingCreated:
                clientModel->addVoting(ser::parseMessage<Voting>(messageText));
                break;
            case VotingDeleted:
                clientModel->removeVoting(ser::parseMessage<int>(messageText));
                break;
            case LoginOk:
                finishLogin(ser::parseMessage<Votings>(messageText));
                break;
            case RegisterOk:
                finishLogin(ser::parseMessage<Votings>(messageText), true);
                break;
            case MyVote:
                clientModel->setMyVote(ser::parseMessage<Vote>(messageText));
                break;
            default: throw std::bad_function_call{}; //"Invalid message command."
        }
    }
    void finishLogin(const Votings& state, bool save = false) {
        if (save)
            clientModel->setUserId(state.myId);
        for (auto& voting : state.votings)
            clientModel->addVoting(voting);
        for (auto& myVote : state.myVotes)
            clientModel->setMyVote(myVote);
        connectionCreated();
    }
};

#endif // VOTESERVERCONNECTION_H
