#include "votingwidget.h"
#include "ui_votingwidget.h"

VotingWidget::VotingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VotingWidget)
{
    ui->setupUi(this);
}

VotingWidget::~VotingWidget()
{
    delete ui;
}

void VotingWidget::setServerMessageQueue(VoteServerConnection *serverConn)
{
    this->serverConn = serverConn;
}

void VotingWidget::setVoting(const Voting &voting)
{
    votingId = voting.id;
    ui->ownerLabel->setText(QString::fromStdString(std::string("by ") + voting.owner));
    ui->titleLabel->setText(QString::fromStdString(voting.title));
    ui->descriptionLabel->setText(QString::fromStdString(voting.description));
    int i = 0;
    for (auto optionBtn : { ui->optionBtn1, ui->optionBtn2, ui->optionBtn3, ui->optionBtn4, ui->optionBtn5 }) {
        if (i < voting.options.size()) {
            optionBtn->show();
            optionBtn->setText(QString::fromStdString(voting.options[i] + " (" + std::to_string(voting.results[i]) + ")"));
        }
        else {
            optionBtn->hide();
        }
        i++;
    }
}

int VotingWidget::getVotingId() const
{
    return votingId;
}

void VotingWidget::setMyVote(int myVote)
{
    enableVoteSelect = false;
    int i = 0;
    for (auto optionBtn : { ui->optionBtn1, ui->optionBtn2, ui->optionBtn3, ui->optionBtn4, ui->optionBtn5 }) {
        //if (myVote == i)
        //    programaticalVoteSelect = true;
        optionBtn->setAutoExclusive(false);
        optionBtn->setChecked(myVote == i);
        optionBtn->setAutoExclusive(true);
        i++;
    }
    enableVoteSelect = true;
}

void VotingWidget::optionBtnChanged(int option)
{
    if (enableVoteSelect)
        serverConn->vote(Vote{votingId, option});
}

void VotingWidget::on_deleteButton_clicked()
{
    serverConn->deleteVoting(votingId);
}

void VotingWidget::on_optionBtn1_toggled(bool checked)
{
    if (checked)
        optionBtnChanged(0);
}

void VotingWidget::on_optionBtn2_toggled(bool checked)
{
    if (checked)
        optionBtnChanged(1);
}

void VotingWidget::on_optionBtn3_toggled(bool checked)
{
    if (checked)
        optionBtnChanged(2);
}

void VotingWidget::on_optionBtn4_toggled(bool checked)
{
    if (checked)
        optionBtnChanged(3);
}

void VotingWidget::on_optionBtn5_toggled(bool checked)
{
    if (checked)
        optionBtnChanged(4);
}
