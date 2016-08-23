#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <algorithm>
#include <QInputDialog>
#include <QPushButton>
#include "newvotingdialog.h"
#include "votingwidget.h"
#include "voteserverconnection.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QInputDialog dialog;
    dialog.setLabelText(tr("Enter the server address and port"));
    if(dialog.exec() == QDialog::Accepted)
    {
        QStringList conn = dialog.textValue().split(':');

        userChannel = new QTextChannel{this};
        userChannel->Connect(conn[0], conn[1].toInt(), [this]{
            model = new VoteClientModel{};
            serverConn = new VoteServerConnection{model, userChannel};

            serverConn->connectionCreated = [this](){
                model = serverConn->voteClientModel();
                model->votingAdded = [this](int voteId) { onVotingAdded(voteId); };
                model->votingChanged = [this](int voteId) { onVotingChanged(voteId); };
                model->votingRemoved = [this](int voteId) { onVotingRemoved(voteId); };
                init();
                statusBar()->showMessage(tr("Welcome %1").arg(QString::fromStdString(model->getMyName())), 5000);
            };
            serverConn->startConnection();
        });
    }
    else
    {
        QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete serverConn;
    delete model;
    //delete userChannel;
}

void MainWindow::init()
{
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    auto votings = model->getVotings();
    std::sort(votings.begin(), votings.end(), [](const VoteClientModel::VotingModel& a, const VoteClientModel::VotingModel& b){
        return b.voting.id < a.voting.id;
    });
    for (auto& voting : votings) {
        QListWidgetItem* item = new QListWidgetItem(); //QString::number(votings[i].voting.id)
        ui->listWidget->addItem(item);
        VotingWidget* votingWidget = new VotingWidget(this);
        votingWidget->setServerMessageQueue(serverConn);
        votingWidget->setVoting(voting.voting);
        votingWidget->setMyVote(voting.myVote);
        item->setSizeHint(QSize(390, 180));
        ui->listWidget->setItemWidget(item, votingWidget);
    }
}

void MainWindow::onVotingAdded(int voteId)
{
    int row;
    for(row = 0; row < ui->listWidget->count(); row++)
    {
         QListWidgetItem* listItem = ui->listWidget->item(row);
         VotingWidget* votingWidget = (VotingWidget*)ui->listWidget->itemWidget(listItem);
         if (votingWidget->getVotingId() < voteId)
             break;
    }
    auto voting = model->getVoting(voteId);
    QListWidgetItem* item = new QListWidgetItem();
    ui->listWidget->insertItem(row, item);
    VotingWidget* votingWidget = new VotingWidget(this);
    votingWidget->setServerMessageQueue(serverConn);
    votingWidget->setVoting(voting.voting);
    item->setSizeHint(QSize(390, 180));
    ui->listWidget->setItemWidget(item, votingWidget);
}

void MainWindow::onVotingChanged(int voteId)
{
    for(int row = 0; row < ui->listWidget->count(); row++)
    {
         QListWidgetItem* listItem = ui->listWidget->item(row);
         VotingWidget* votingWidget = (VotingWidget*)ui->listWidget->itemWidget(listItem);
         if (votingWidget->getVotingId() == voteId)
         {
             auto voting = model->getVoting(voteId);
             votingWidget->setVoting(voting.voting);
         }
    }
}

void MainWindow::onVotingRemoved(int voteId)
{
    for(int row = 0; row < ui->listWidget->count(); row++)
    {
         QListWidgetItem* listItem = ui->listWidget->item(row);
         VotingWidget* votingWidget = (VotingWidget*)ui->listWidget->itemWidget(listItem);
         if (votingWidget->getVotingId() == voteId)
         {
             delete ui->listWidget->takeItem(row);
             break;
         }
    }
}

void MainWindow::on_action_New_voting_triggered()
{
    NewVotingDialog dialog;
    if(dialog.exec() == QDialog::Accepted)
    {
        serverConn->createVoting(dialog.getVoting(model));
        statusBar()->showMessage(tr("Voting added"), 5000);
    }
}

void MainWindow::on_action_Edit_nickname_triggered()
{
    QInputDialog dialog;
    dialog.setLabelText(tr("Set your new nickname"));
    if(dialog.exec() == QDialog::Accepted)
    {
        QString nickname = dialog.textValue();
        model->setMyName(nickname.toStdString());
        statusBar()->showMessage(tr("Nickname set to: %1").arg(nickname), 5000);
    }
}
