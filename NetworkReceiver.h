#ifndef NETWORKRECEIVER_H
#define NETWORKRECEIVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonParseError>

class NetworkReceiver : public QObject
{
    Q_OBJECT
public:
    explicit NetworkReceiver(QObject *parent = nullptr);
    ~NetworkReceiver();

    bool startListening(quint16 port);
    void stopListening();

signals:
    void configurationReceived(const QJsonDocument &config);
    void errorOccurred(const QString &errorMsg);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    QTcpServer *server;
    QByteArray incomingData;
};

#endif // NETWORKRECEIVER_H
