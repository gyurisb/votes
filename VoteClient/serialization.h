#ifndef SERIALIZATION_H
#define SERIALIZATION_H

namespace ser
{
    template <typename T>
    struct Parser;

    template <typename T>
    struct Parser<std::vector<T>> {
        static std::vector<T> parseMessageData(std::istream& is) {
            std::vector<T> res;
            std::string word;
            is >> word;
            if (word != "[")
                throw std::invalid_argument{"Invalid vector format"};
            while (true) {
                is >> word;
                if (word == "]")
                    break;
                int word_size = word.size();
                is.seekg(-word_size, std::istream::cur);
                res.push_back(Parser<T>::parseMessageData(is));
            }
            return res;
        }
    };
    template <>
    struct Parser<int> {
        static int parseMessageData(std::istream& is) {
            int res;
            is >> res;
            return res;
        }
    };
    template <>
    struct Parser<std::string> {
        static std::string parseMessageData(std::istream& is) {
            std::ostringstream os;
            while (true) {
                std::string word;
                is >> word;
                os << word << ' ';
                if (word[word.size() - 1] == '"')
                    break;
            }
            std::string res = os.str();
            return res.substr(1, res.size() - 3);
        }
    };
    template <>
    struct Parser<Vote> {
        static Vote parseMessageData(std::istream& is) {
            Vote res;
            res.votingId = Parser<int>::parseMessageData(is);
            res.option = Parser<int>::parseMessageData(is);
            return res;
        }
    };
    template <>
    struct Parser<VotingUpdate> {
        static VotingUpdate parseMessageData(std::istream& is) {
            VotingUpdate res;
            res.id = Parser<int>::parseMessageData(is);
            res.results = Parser<std::vector<int>>::parseMessageData(is);
            return res;
        }
    };
    template <>
    struct Parser<Voting> {
        static Voting parseMessageData(std::istream& is) {
            Voting res;
            res.id = Parser<int>::parseMessageData(is);
            res.owner = Parser<std::string>::parseMessageData(is);
            res.title = Parser<std::string>::parseMessageData(is);
            res.description = Parser<std::string>::parseMessageData(is);
            res.imageUrl = Parser<std::string>::parseMessageData(is);
            res.options = Parser<std::vector<std::string>>::parseMessageData(is);
            res.expiration = Parser<int>::parseMessageData(is);
            res.results = Parser<std::vector<int>>::parseMessageData(is);
            return res;
        }
    };
    template <>
    struct Parser<Votings> {
        static Votings parseMessageData(std::istream& is) {
            Votings res;
            res.myId = Parser<int>::parseMessageData(is);
            res.votings = Parser<std::vector<Voting>>::parseMessageData(is);
            res.myVotes = Parser<std::vector<Vote>>::parseMessageData(is);
            return res;
        }
    };

    template <typename T>
    T parseMessage(const std::string& message)
    {
        std::istringstream is(message);
        int command;
        is >> command;
        return Parser<T>::parseMessageData(is);
    }

    template <typename T>
    struct Composer;

    template <typename T>
    struct Composer<std::vector<T>> {
        static void composeMessageData(std::ostream& os, const std::vector<T>& data) {
            os << "[ ";
            for (const T& item : data) {
                Composer<T>::composeMessageData(os, item);
            }
            os << "] ";
        }
    };
    template <>
    struct Composer<int> {
        static void composeMessageData(std::ostream& os, int data) {
            os << data << " ";
        }
    };
    template <>
    struct Composer<std::string> {
        static void composeMessageData(std::ostream& os, const std::string& data) {
            os << '"' << data << "\" ";
        }
    };
    template <>
    struct Composer<Vote> {
        static void composeMessageData(std::ostream& os, const Vote& data) {
            Composer<int>::composeMessageData(os, data.votingId);
            Composer<int>::composeMessageData(os, data.option);
        }
    };
    template <>
    struct Composer<Voting> {
        static void composeMessageData(std::ostream& os, const Voting& data) {
            Composer<int>::composeMessageData(os, data.id);
            Composer<std::string>::composeMessageData(os, data.owner);
            Composer<std::string>::composeMessageData(os, data.title);
            Composer<std::string>::composeMessageData(os, data.description);
            Composer<std::string>::composeMessageData(os, data.imageUrl);
            Composer<std::vector<std::string>>::composeMessageData(os, data.options);
            Composer<int>::composeMessageData(os, data.expiration);
            Composer<std::vector<int>>::composeMessageData(os, data.results);
        }
    };
    template <>
    struct Composer<VotingUpdate> {
        static void composeMessageData(std::ostream& os, const VotingUpdate& data) {
            Composer<int>::composeMessageData(os, data.id);
            Composer<std::vector<int>>::composeMessageData(os, data.results);
        }
    };
    template <>
    struct Composer<Votings> {
        static void composeMessageData(std::ostream& os, const Votings& data) {
            Composer<int>::composeMessageData(os, data.myId);
            Composer<std::vector<Voting>>::composeMessageData(os, data.votings);
            Composer<std::vector<Vote>>::composeMessageData(os, data.myVotes);
        }
    };

    template <typename T>
    std::string composeMessage(int command, const T& data) {
        std::ostringstream os;
        os << command << " ";
        Composer<T>::composeMessageData(os, data);
        return os.str();
    }
}

#endif // SERIALIZATION_H
