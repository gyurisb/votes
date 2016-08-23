#-------------------------------------------------
#
# Project created by QtCreator 2016-05-08T20:19:55
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VoteClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    newvotingdialog.cpp \
    votingwidget.cpp

HEADERS  += mainwindow.h \
    newvotingdialog.h \
    votingwidget.h \
    voteobjects.h \
    itextchannel.h \
    voteserverconnection.h \
    voteclientmodel.h \
    serialization.h \
    voteserver.h \
    qtextchannel.h

FORMS    += mainwindow.ui \
    newvotingdialog.ui \
    votingwidget.ui
