#ifndef QTEXTCHANNEL_H
#define QTEXTCHANNEL_H

#include <QObject>
#include <QtNetwork>
#include <QTcpSocket>
#include <sstream>
#include "itextchannel.h"

class QTextChannel : public QObject, public ITextChannel
{
    Q_OBJECT
public:
    enum ConnStatus { Error, Connecting, Connected, Disconnected };
public:
    explicit QTextChannel(QObject *parent = 0) : QObject(parent) {
        m_pSocket = NULL;
    }
    virtual ~QTextChannel() {
        disconnect(this, SLOT(slotDisconnected()));
        delete m_pSocket;
    }
    void Connect(QString addr, int port, std::function<void()> connected = []{}) {

        if (m_pSocket) return;
        m_pSocket = new QTcpSocket();


        emit signalConnectionStatus(Connecting);
        connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
        connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));

        m_pSocket->connectToHost(addr, port);

        if (!m_pSocket->waitForConnected(5000)) {
            delete m_pSocket;
            m_pSocket = NULL;
            emit signalConnectionStatus(Error);
            return;
        }

        connected();
        emit signalConnectionStatus(Connected);
    }

    virtual void sendMessage(std::string data) {
        std::cout << "send: " << data << std::endl;
        m_pSocket->write(data.c_str());
        m_pSocket->write("\n");
    }

signals:
    void signalConnectionStatus(int status);

public slots:
    void slotReadyRead() {
        while (m_pSocket->bytesAvailable()) {
            QByteArray buf = m_pSocket->read(1);
            if (buf[0] == '\n'){
                std::string msg = tempStream.str();
                tempStream.str("");
                std::cout << "recv: " << msg << std::endl;
                messageReceived(msg);
            }
            else {
                tempStream << buf[0];
            }
        }
    }
    void slotDisconnected() {
        // Kikotjuk a socket lezarasanak erzekeleset, mert kulonben a sajatunkat is
        // erzekelnenk.
        disconnect(this, SLOT(slotDisconnected()));
        // Ha letezik meg kliens socket akkor lezarjuk.
        if (m_pSocket) {
            m_pSocket->deleteLater();
            m_pSocket = NULL;
        }
        // Jelezzuk a kapcsolat allapotanak valtozasat.
        emit signalConnectionStatus(Disconnected);
    }

protected:
    QTcpSocket* m_pSocket;
    std::ostringstream tempStream;
};

#endif // QTEXTCHANNEL_H
