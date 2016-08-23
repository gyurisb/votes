#ifndef VOTECLIENTMODEL_H
#define VOTECLIENTMODEL_H

#include "voteobjects.h"

class VoteClientModel final {
public:
    struct VotingModel {
        Voting voting;
        int myVote;
    };
private:
    /* Model data */
    int myId;
    std::string myName;
    std::map<int, VotingModel> votings;
public:
    /* Events */
    std::function<void(int)> votingAdded = [](int){};
    std::function<void(int)> votingRemoved = [](int){};
    std::function<void(int)> votingChanged = [](int){};

    explicit VoteClientModel() {
        std::ifstream fs_id("user_id");
        if (fs_id.good()) {
            fs_id >> myId;
            fs_id.close();
        }
        else {
            myId = -1;
        }
        std::ifstream fs_name("user_name");
        if (fs_name.good()) {
            std::getline(fs_name, myName);
            fs_name.close();
        }
        else if (myId != -1) {
            myName = std::string{"user"} + std::to_string(myId);
        }
    }
    VoteClientModel(const VoteClientModel&) = delete;

    /* Model setter functions */
    void setUserId(int id) {
        myId = id;
        std::ofstream fs("user_id");
        fs << myId;
        fs.close();
        if (myName == "")
            myName = std::string{"user"} + std::to_string(myId);
    }
    void setMyName(std::string name) {
        myName = name;
        std::ofstream fs("user_name");
        fs << myName << std::endl;
        fs.close();
    }
    void addVoting(const Voting& voting) {
        VotingModel model;
        model.voting = voting;
        model.myVote = -1;
        votings[voting.id] = model;
        votingAdded(voting.id);
    }
    void removeVoting(int voteId) {
        votings.erase(voteId);
        votingRemoved(voteId);
    }
    void updateVoting(const VotingUpdate& updateModel) {
        votings[updateModel.id].voting.results = updateModel.results;
        votingChanged(updateModel.id);
    }
    void setMyVote(const Vote& vote) {
        votings[vote.votingId].myVote = vote.option;
    }

    /* Model getter functions */
    std::string getMyName() const {
        return myName;
    }
    const VotingModel& getVoting(int voteId) const {
        return votings.at(voteId);
    }
    std::vector<VotingModel> getVotings() const {
        std::vector<VotingModel> model;
        model.reserve(votings.size());
        for (auto& voting : votings)
            model.push_back(voting.second);
        return model;
    }
    bool registered() const {
        return myId != -1;
    }
    int userId() const {
        return myId;
    }
};


#endif // VOTECLIENTMODEL_H
