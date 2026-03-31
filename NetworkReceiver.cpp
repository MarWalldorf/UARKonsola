#include "NetworkReceiver.h"

NetworkReceiver::NetworkReceiver(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &NetworkReceiver::onNewConnection);
}

NetworkReceiver::~NetworkReceiver()
{
    stopListening();
}

bool NetworkReceiver::startListening(quint16 port)
{
    
    if (server->isListening()) server->close();

    if (server->listen(QHostAddress::Any, port)) {
        return true;
    } else {
        emit errorOccurred("Nie można uruchomić serwera: " + server->errorString());
        return false;
    }
}

void NetworkReceiver::stopListening()
{
    if (server->isListening()) {
        server->close();
    }
}

void NetworkReceiver::onNewConnection()
{
    QTcpSocket *clientConnection = server->nextPendingConnection();

   
    incomingData.clear();

    connect(clientConnection, &QTcpSocket::readyRead, this, &NetworkReceiver::onReadyRead);
    connect(clientConnection, &QTcpSocket::disconnected, this, &NetworkReceiver::onClientDisconnected);

    
    connect(clientConnection, &QTcpSocket::errorOccurred, this, [this, clientConnection](QAbstractSocket::SocketError){
        emit errorOccurred("Błąd połączenia: " + clientConnection->errorString());
    });
}

void NetworkReceiver::onReadyRead()
{
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (clientConnection) {
        
        incomingData.append(clientConnection->readAll());
    }
}

void NetworkReceiver::onClientDisconnected()
{
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (!clientConnection) return;

    
    if (!incomingData.isEmpty()) {
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(incomingData, &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            if (!doc.isNull() && doc.isObject()) {
                emit configurationReceived(doc);
            }
        } else {
            
            emit errorOccurred("Błąd struktury JSON: " + parseError.errorString());
        }
    }

    incomingData.clear();
    clientConnection->deleteLater();
}
