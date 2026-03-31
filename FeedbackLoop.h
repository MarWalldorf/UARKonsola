#pragma once
#include "Regulator_PID.h"
#include "ARX_Model.h"
#include "Generator.h"
#include <array>
class FeedbackLoop
{
private:
    ARX_Model arx_model;
    Regulator_PID regulator;
    Generator generator;
    double lastY = 0;
    double lastE = 0;
    double lastU = 0;
    bool useGen = false;
    double wzadane = 0;

    double ObliczSygnalWejsciowy();
public:
    FeedbackLoop(ARX_Model model, Regulator_PID pid,Generator gen);
    ~FeedbackLoop(){};
    Generator& getGenerator();
    ARX_Model& getARXModel();
    Regulator_PID& getRegulator();
    std::array<double,7> Symuluj();
    double UruchomSymulacje(double e_);
    void setUseGen(bool);
    void setWZadane(double wz);
    double GetWz();
    void Reset();
};

