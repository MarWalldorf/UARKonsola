#pragma once
#include "FeedbackLoop.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject.h>
#include <vector>
#include <QTimer>

class MenagerSymulacji : public QObject
{
    Q_OBJECT
    FeedbackLoop loop;
    QTimer* timer;
    int timerInteval=200;


public:
    MenagerSymulacji(FeedbackLoop loop, QObject*);
    ~MenagerSymulacji();

    // Settery parametrów modelu ARX
    void SetAParams(std::vector<double> a_);
    void SetBParams(std::vector<double> b_);
    void SetK(int k_);
    void SetZ(double z_);
    void SetULimits(bool state, double min_, double max_);
    void SetYLimits(bool state, double min_, double max_);
    void ARX_Clear();
    void Reset_Regulator();

    // Gettery ARX
    ARX_Model& GetARX();

    //Settery parametrów regulatora PID
    void SetKp(double Kp_);
    void SetTi(double Ti_);
    void SetTd(double Td_);
    void SetIType(bool I_type_);
    void ClearIntegral();
    void Clear_Derivative();
    double getKp();
    double getTi();
    double getTd();


    //Settery parametrów generatora sygnału
    void SetGeneratorType(bool type_); // false - sin, true - square
    void SetGeneratorParameters(int T_);
    void SetGeneratorAmplitude(double A_);
    void SetGeneratorShift(double S_);
    void SetGeneratorPulseWidth(double p_);
    void ResetGeneratorCounter();
    void setUseGen(bool);
    void setWZadane(double wz);

    // Uruchomienie symulacji
    std::array<double,7> Symuluj();
    void ChangeRunningState();
    bool GetRunningState();
    void setInterval(int);
    void setTimerState(bool);
    int getInterval();

    //Zarzadzanie plikiem
    QJsonDocument Serialize();
    void deserialize(QJsonDocument doc);

    //Reset
    void Reset();
private slots:
    void TriggerSim();
signals:
    void UpdateSim(std::array<double,7>);
};

