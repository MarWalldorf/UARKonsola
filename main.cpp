#include <QCoreApplication>
#include <QTextStream>

#include "MenagerSymulacji.h"
#include "NetworkSender.h"
#include "NetworkReceiver.h"

void runSender(MenagerSymulacji &manager, const QString &ip, quint16 port) {
    QTextStream qout(stdout);


    manager.SetKp(1.75);
    manager.SetTi(2.50);
    manager.SetTd(0.35);
    manager.SetAParams({1.0, -0.6});
    manager.SetBParams({0.5, 0.1});
    manager.SetZ(0.01);

    QJsonDocument doc = manager.Serialize();

    qout << "\n--- Konfiguracja przygotowana do wyslania ---\n";
    qout << "Wysylam do: " << ip << ":" << port << "...\n";
    qout.flush();

    NetworkSender *sender = new NetworkSender();

    QObject::connect(sender, &NetworkSender::dataSent, []() {
        QTextStream localOut(stdout);
        localOut << "[SUKCES] Konfiguracja zostala pomyslnie wyslana!\n";
        localOut << "Dane dotarly do odbiorcy. Mozesz zamknac program.\n";
        localOut.flush();
    });

    QObject::connect(sender, &NetworkSender::errorOccurred, [](const QString &err) {
        QTextStream localOut(stdout);
        localOut << "[BLAD SIECI] Problem z wyslaniem: " << err << "\n";
        localOut << "Upewnij sie, ze Odbiorca (Serwer) jest uruchomiony i IP jest poprawne.\n";
        localOut.flush();
    });

    sender->sendConfiguration(ip, port, doc);
}

void runReceiver(MenagerSymulacji &manager, quint16 port) {
    QTextStream qout(stdout);

    NetworkReceiver *receiver = new NetworkReceiver();

    QObject::connect(receiver, &NetworkReceiver::configurationReceived, [&manager](const QJsonDocument &doc) {
        QTextStream localOut(stdout);
        localOut << "[SUKCES] Odebrano nowa konfiguracje z sieci!\n";

        manager.deserialize(doc);

        localOut << "Zaktualizowane parametry \n";
        localOut << "Regulator PID:\n";
        localOut << "  Kp: " << manager.getKp() << "\n";
        localOut << "  Ti: " << manager.getTi() << "\n";
        localOut << "  Td: " << manager.getTd() << "\n";

        localOut << "Model ARX:\n";
        localOut << "  Szum (Z): " << manager.GetARX().getZ() << "\n";
        localOut << "  Opoznienie (k): " << manager.GetARX().getK() << "\n";

        localOut << "\nCzekam na kolejne pakiety...\n";
        localOut.flush();
    });

    QObject::connect(receiver, &NetworkReceiver::errorOccurred, [](const QString &err) {
        QTextStream localOut(stdout);
        localOut << "\n[BLAD ODBIORCY] " << err << "\n";
        localOut.flush();
    });

    if (receiver->startListening(port)) {
        qout << "\n[INFO] Serwer zostal pomyslnie uruchomiony!\n";
        qout << "[INFO] Nasluchuje na porcie " << port << "... Czekam na dane.\n";
        qout.flush();
    } else {
        qout << "\n[KRYTYCZNY BLAD] Nie mozna otworzyc portu " << port << "!\n";
        qout.flush();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream qout(stdout);

    FeedbackLoop loop(ARX_Model(std::vector<double>(), std::vector<double>()), Regulator_PID(0,0,0,0), Generator());
    MenagerSymulacji manager(loop, nullptr);


    bool uruchomJakoOdbiorca = false;
    quint16 port = 12345;
    QString ipOdbiorcy = "192.168.1.21";



    qout << "Aplikacja Sieciowa ARX & PID\n";

    if (uruchomJakoOdbiorca) {
        qout << "TRYB: ODBIORCA (Serwer) <<<\n";
        qout.flush();
        runReceiver(manager, port);
    } else {
        qout << "TRYB: NADAWCA (Klient) <<<\n";
        qout.flush();
        runSender(manager, ipOdbiorcy, port);
    }

    return a.exec();
}
