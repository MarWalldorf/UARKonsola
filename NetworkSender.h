#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>

class NetworkSender : public QObject
{
    Q_OBJECT
public:
    explicit NetworkSender(QObject *parent = nullptr);

    
    void sendConfiguration(const QString &host, quint16 port, const QJsonDocument &config);

signals:
    void dataSent();
    void errorOccurred(const QString &errorMsg);

private slots:
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
    QByteArray dataToSend;
};

#endif // NETWORKSENDER_H
