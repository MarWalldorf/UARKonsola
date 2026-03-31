#pragma once
#include <vector>
#include <array>
#include <random>
#include "Regulator_PID.h"
#include "NQueue.h"
class ARX_Model
{
private:
    std::vector<double> aParams;
    std::vector<double> bParams;
    NQueue<double> U_valueQueue;
    NQueue<double> Y_valueQueue;
    NQueue<double> k_delayQueue;
    std::normal_distribution<double> distr;
    std::mt19937 random;
    int k;
    double z;
    Regulator_PID* regulator = nullptr;
    double U_Max = 10, U_Min = -10, Y_Max = 10, Y_Min = -10;
    bool Y_MinMax = true, U_MinMax = true;
    double lastSavedU;
public:
    ARX_Model(std::vector<double> a_, std::vector<double> b_, int k_ = 1, double z_ = 0.0);
    double GrindingFunction(double u_);
    double CalcFunction(std::vector<double> arguments, NQueue<double> list);
    void SetU_Limits(bool state, double min_, double max_);
    void SetY_Limits(bool state, double min_, double max_);
    std::tuple<double,double,bool> getULimits();
    std::tuple<double,double,bool> getYLimits();
    void SetAParams(std::vector<double> a_);
    void SetBParams(std::vector<double> b_);
    void SetK(int k_);
    void SetZ(double z_);
    std::tuple<double,double> GetDistr();
    double GetYValue();
    std::vector<double> GetAParams();
    std::vector<double> GetBParams();
    double getZ();
    int getK();
    void ResetQueue();
    double getLastU();
    std::array<int,2> ParamsCounter();
};
