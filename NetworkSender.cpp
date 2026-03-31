#include "NetworkSender.h"

NetworkSender::NetworkSender(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &NetworkSender::onConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &NetworkSender::onError);
}

void NetworkSender::sendConfiguration(const QString &host, quint16 port, const QJsonDocument &config)
{
    // Kompresujemy JSON, aby zajął mniej miejsca w sieci
    dataToSend = config.toJson(QJsonDocument::Compact);

    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
    socket->connectToHost(host, port);
}

void NetworkSender::onConnected()
{
    socket->write(dataToSend);
    socket->waitForBytesWritten();
    socket->disconnectFromHost(); // Rozłączamy po wysłaniu
    emit dataSent();
}

void NetworkSender::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit errorOccurred(socket->errorString());
}
