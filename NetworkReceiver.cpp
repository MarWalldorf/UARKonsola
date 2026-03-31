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
    // Restartujemy serwer, jeśli już działał
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

    // Czyścimy bufor przed nowym połączeniem, aby uniknąć starych śmieci
    incomingData.clear();

    connect(clientConnection, &QTcpSocket::readyRead, this, &NetworkReceiver::onReadyRead);
    connect(clientConnection, &QTcpSocket::disconnected, this, &NetworkReceiver::onClientDisconnected);

    // Opcjonalnie: obsługa błędów gniazda
    connect(clientConnection, &QTcpSocket::errorOccurred, this, [this, clientConnection](QAbstractSocket::SocketError){
        emit errorOccurred("Błąd połączenia: " + clientConnection->errorString());
    });
}

void NetworkReceiver::onReadyRead()
{
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (clientConnection) {
        // Dopisywanie kolejnych paczek danych do bufora
        incomingData.append(clientConnection->readAll());
    }
}

void NetworkReceiver::onClientDisconnected()
{
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (!clientConnection) return;

    // Przetwarzamy dane tylko jeśli cokolwiek odebrano
    if (!incomingData.isEmpty()) {
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(incomingData, &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            if (!doc.isNull() && doc.isObject()) {
                emit configurationReceived(doc);
            }
        } else {
            // To zapobiega crashowi - zamiast wywalić program, wypisze błąd w konsoli
            emit errorOccurred("Błąd struktury JSON: " + parseError.errorString());
        }
    }

    incomingData.clear(); // Kluczowe: czyszczenie po przetworzeniu
    clientConnection->deleteLater();
}
