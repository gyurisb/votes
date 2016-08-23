#include "newvotingdialog.h"
#include "ui_newvotingdialog.h"

NewVotingDialog::NewVotingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewVotingDialog)
{
    ui->setupUi(this);
}

NewVotingDialog::~NewVotingDialog()
{
    delete ui;
}

Voting NewVotingDialog::getVoting(VoteClientModel* model)
{
    Voting voting;
    voting.id = -1;
    voting.owner = model->getMyName();
    voting.title = ui->titleEdit->text().toStdString();
    voting.description = ui->descriptionEdit->toPlainText().toStdString();
    for (auto& optionEdit : { ui->option1Edit, ui->option2Edit, ui->option3Edit, ui->option4Edit, ui->option5Edit }) {
        if (optionEdit->text() != "")
            voting.options.push_back(optionEdit->text().toStdString());
    }
    voting.results = std::vector<int>(voting.options.size());
    voting.expiration = 0;
    return voting;
}
