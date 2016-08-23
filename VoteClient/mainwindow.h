#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "voteserverconnection.h"
#include "voteserver.h"
#include "votingwidget.h"
#include "qtextchannel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_New_voting_triggered();

    void on_action_Edit_nickname_triggered();

private:
    Ui::MainWindow *ui;

    VoteServerConnection* serverConn;
    VoteClientModel* model;
    QTextChannel* userChannel;

    void init();
    void onVotingChanged(int);
    void onVotingAdded(int);
    void onVotingRemoved(int);

};

#endif // MAINWINDOW_H
