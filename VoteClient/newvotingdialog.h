#ifndef NEWVOTINGDIALOG_H
#define NEWVOTINGDIALOG_H

#include <QDialog>
#include "voteserverconnection.h"

namespace Ui {
class NewVotingDialog;
}

class NewVotingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewVotingDialog(QWidget *parent = 0);
    ~NewVotingDialog();

    Voting getVoting(VoteClientModel* model);

private:
    Ui::NewVotingDialog *ui;
};

#endif // NEWVOTINGDIALOG_H
