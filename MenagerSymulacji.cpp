#include "MenagerSymulacji.h"
#include <QJsonArray>
#include <QJsonObject>

MenagerSymulacji::MenagerSymulacji(FeedbackLoop loop_, QObject *parent)
    : loop(loop_), QObject(parent) {
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval(timerInteval);
    connect(timer, &QTimer::timeout, this, &MenagerSymulacji::TriggerSim);
}

void MenagerSymulacji::TriggerSim() {
    std::array<double, 7> out;
    out = Symuluj();
    emit UpdateSim(out);
}

// POPRAWKA: Usunięto ręczne wywołanie destruktora loop.~FeedbackLoop()
// C++ automatycznie niszczy składowe klasy w odpowiedniej kolejności.
MenagerSymulacji::~MenagerSymulacji() {
    // Destruktor pozostaje pusty, chyba że alokowałeś coś przez 'new' bez rodzica QObject.
}

// Settery parametrów modelu ARX
void MenagerSymulacji::SetAParams(std::vector<double> a_) {
    loop.getARXModel().SetAParams(a_);
}

void MenagerSymulacji::SetBParams(std::vector<double> b_) {
    loop.getARXModel().SetBParams(b_);
}

void MenagerSymulacji::SetK(int k_) {
    loop.getARXModel().SetK(k_);
}

void MenagerSymulacji::SetZ(double z_) {
    loop.getARXModel().SetZ(z_);
}

void MenagerSymulacji::SetULimits(bool state, double min_, double max_) {
    loop.getARXModel().SetU_Limits(state, min_, max_);
}

void MenagerSymulacji::SetYLimits(bool state, double min_, double max_) {
    loop.getARXModel().SetY_Limits(state, min_, max_);
}

void MenagerSymulacji::ARX_Clear() {
    loop.getARXModel().ResetQueue();
}

// Gettery ARX
ARX_Model& MenagerSymulacji::GetARX() {
    return loop.getARXModel();
}

void MenagerSymulacji::Reset_Regulator() {
    loop.getRegulator().set_Kp(0.0);
    loop.getRegulator().set_Td(0.0);
    loop.getRegulator().set_Ti(0.0);
}

// Settery parametrów regulatora PID
void MenagerSymulacji::SetKp(double Kp_) {
    loop.getRegulator().set_Kp(Kp_);
}

void MenagerSymulacji::SetTi(double Ti_) {
    loop.getRegulator().set_Ti(Ti_);
}

void MenagerSymulacji::SetTd(double Td_) {
    loop.getRegulator().set_Td(Td_);
}

void MenagerSymulacji::SetIType(bool I_type_) {
    loop.getRegulator().set_IType(I_type_);
}

void MenagerSymulacji::ClearIntegral() {
    loop.getRegulator().Clear_Integral();
}

void MenagerSymulacji::Clear_Derivative() {
    loop.getRegulator().Clear_Derivative();
}

// Settery parametrów generatora sygnału
void MenagerSymulacji::SetGeneratorType(bool type_) {
    loop.getGenerator().SetGeneratorType(type_);
}

void MenagerSymulacji::SetGeneratorParameters(int T_) {
    loop.getGenerator().setParameters(T_);
}

void MenagerSymulacji::SetGeneratorAmplitude(double A_) {
    loop.getGenerator().setAmplitude(A_);
}

void MenagerSymulacji::SetGeneratorShift(double S_) {
    loop.getGenerator().setShift(S_);
}

void MenagerSymulacji::SetGeneratorPulseWidth(double p_) {
    loop.getGenerator().setPulseWidth(p_);
}

void MenagerSymulacji::ResetGeneratorCounter() {
    loop.getGenerator().resetCounter();
}

void MenagerSymulacji::setUseGen(bool state) {
    loop.setUseGen(state);
}

void MenagerSymulacji::setWZadane(double wz) {
    loop.setWZadane(wz);
}

// Uruchomienie symulacji
std::array<double, 7> MenagerSymulacji::Symuluj() {
    return loop.Symuluj();
}

// Zarzadzanie plikiem
QJsonDocument MenagerSymulacji::Serialize() {
    QJsonObject ModelArx;
    QJsonArray ModelAParams, ModelBParams;

    auto [Umin, Umax, Ustate] = loop.getARXModel().getULimits();
    auto [Ymin, Ymax, YState] = loop.getARXModel().getYLimits();

    ModelArx["z"] = loop.getARXModel().getZ();
    ModelArx["k"] = loop.getARXModel().getK();
    ModelArx["Umin"] = Umin;
    ModelArx["Umax"] = Umax;
    ModelArx["Ustate"] = Ustate;
    ModelArx["Ymin"] = Ymin;
    ModelArx["Ymax"] = Ymax;
    ModelArx["Ystate"] = YState;

    for (const auto &item : loop.getARXModel().GetAParams()) {
        ModelAParams.append(item);
    }
    for (const auto &item : loop.getARXModel().GetBParams()) {
        ModelBParams.append(item);
    }
    ModelArx["AParams"] = ModelAParams;
    ModelArx["BParams"] = ModelBParams;

    QJsonObject Regulator;
    auto [Kp, Td, Ti, integral_type] = loop.getRegulator().GetRegParams();
    Regulator["Kp"] = Kp;
    Regulator["Td"] = Td;
    Regulator["Ti"] = Ti;
    Regulator["integral_Type"] = integral_type;

    QJsonObject Generator;
    auto [T, A, S, p] = loop.getGenerator().GetGeneratorParams();
    Generator["T"] = T;
    Generator["A"] = A;
    Generator["S"] = S;
    Generator["p"] = p;
    Generator["gen_type"] = loop.getGenerator().GetGeneratorType();

    QJsonObject RootFile;
    RootFile["ModelArx"] = ModelArx;
    RootFile["Regulator"] = Regulator;
    RootFile["Generator"] = Generator;
    RootFile["WartoscZadana"] = loop.GetWz();
    RootFile["TimerInterval"] = timerInteval;

    return QJsonDocument(RootFile);
}

// POPRAWKA: Dodano walidację pól JSON, aby zapobiec błędom MaxSize
void MenagerSymulacji::deserialize(QJsonDocument doc) {
    if (doc.isNull() || !doc.isObject()) return;

    QJsonObject root = doc.object();

    // Pobieranie wartości zadanej i timera
    if (root.contains("WartoscZadana")) loop.setWZadane(root["WartoscZadana"].toDouble());
    if (root.contains("TimerInterval")) {
        timerInteval = root["TimerInterval"].toInt();
        setInterval(timerInteval);
    }

    // Generator
    if (root.contains("Generator")) {
        QJsonObject generator = root["Generator"].toObject();
        loop.getGenerator().SetGeneratorType(generator["gen_type"].toBool());
        loop.getGenerator().setParameters(generator["T"].toInt());
        loop.getGenerator().setAmplitude(generator["A"].toDouble());
        loop.getGenerator().setShift(generator["S"].toDouble());
        loop.getGenerator().setPulseWidth(generator["p"].toDouble());
    }

    // Regulator
    if (root.contains("Regulator")) {
        QJsonObject regulator = root["Regulator"].toObject();
        loop.getRegulator().set_IType(regulator["integral_Type"].toBool());
        loop.getRegulator().set_Kp(regulator["Kp"].toDouble());
        loop.getRegulator().set_Td(regulator["Td"].toDouble());
        loop.getRegulator().set_Ti(regulator["Ti"].toDouble());
    }

    // ModelARX
    if (root.contains("ModelArx")) {
        QJsonObject model = root["ModelArx"].toObject();
        QJsonArray AParams = model["AParams"].toArray();
        QJsonArray BParams = model["BParams"].toArray();

        std::vector<double> AParamsVector, BParamsVector;
        for (QJsonValueRef item : AParams) AParamsVector.push_back(item.toDouble());
        for (QJsonValueRef item : BParams) BParamsVector.push_back(item.toDouble());

        loop.getARXModel().SetAParams(AParamsVector);
        loop.getARXModel().SetBParams(BParamsVector);
        loop.getARXModel().SetU_Limits(model["Ustate"].toBool(), model["Umin"].toDouble(), model["Umax"].toDouble());
        loop.getARXModel().SetY_Limits(model["Ystate"].toBool(), model["Ymin"].toDouble(), model["Ymax"].toDouble());
        loop.getARXModel().SetK(model["k"].toInt());
        loop.getARXModel().SetZ(model["z"].toDouble());
    }
}

void MenagerSymulacji::Reset() {
    loop.Reset();
}

double MenagerSymulacji::getKp() { return loop.getRegulator().getKp(); }
double MenagerSymulacji::getTi() { return loop.getRegulator().getTi(); }
double MenagerSymulacji::getTd() { return loop.getRegulator().getTd(); }

void MenagerSymulacji::setInterval(int ms) {
    timerInteval = ms;
    if (timer) timer->setInterval(timerInteval);
}

void MenagerSymulacji::setTimerState(bool state) {
    if (state) timer->start();
    else timer->stop();
}

int MenagerSymulacji::getInterval() { return timerInteval; }
