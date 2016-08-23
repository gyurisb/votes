#ifndef VOTINGWIDGET_H
#define VOTINGWIDGET_H

#include <QWidget>
#include "voteserverconnection.h"

namespace Ui {
class VotingWidget;
}

class VotingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VotingWidget(QWidget *parent = 0);
    ~VotingWidget();

    void setServerMessageQueue(VoteServerConnection* serverConn);
    void setVoting(const Voting& voting);
    int getVotingId() const;
    void setMyVote(int myVote);

private slots:
    void on_optionBtn1_toggled(bool checked);
    void on_optionBtn2_toggled(bool checked);
    void on_optionBtn3_toggled(bool checked);
    void on_optionBtn4_toggled(bool checked);
    void on_optionBtn5_toggled(bool checked);
    void on_deleteButton_clicked();

private:
    Ui::VotingWidget *ui;
    VoteServerConnection* serverConn;
    int votingId;
    bool enableVoteSelect = true;

    void optionBtnChanged(int option);
};

#endif // VOTINGWIDGET_H
