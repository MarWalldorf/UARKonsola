#include "ARX_Model.h"
#define Simulate GrindingFunction

ARX_Model::ARX_Model(std::vector<double> a_, std::vector<double> b_, int k_, double z_)
    : aParams(a_), bParams(b_)
{
    k = k_ < 1 ? 1 : k_;
    for (int i = 0; i < k; i++) k_delayQueue.Enqueue(0);
    for (int i = 0; i < bParams.size(); i++) U_valueQueue.Enqueue(0);
    for (int i = 0; i < aParams.size(); i++) Y_valueQueue.Enqueue(0);
    z = z_ >= 0 ? z_ : fabs(z_);
    if (z > 0)
    {
        distr = std::normal_distribution<double>(0.0, z);
        random = std::mt19937(std::random_device{}());
    }
}

double ARX_Model::GrindingFunction(double u_)
{
    if (U_MinMax) u_ = u_ > U_Max ? U_Max : u_ < U_Min ? U_Min : u_;
    lastSavedU = u_;
    double Y_Val = 0.0;
    U_valueQueue.pop_front();
    U_valueQueue.Enqueue(k_delayQueue.Dequeue());
    k_delayQueue.Enqueue(u_);

    Y_Val = CalcFunction(bParams, U_valueQueue) - CalcFunction(aParams, Y_valueQueue);
    if (z > 0)
    {
        Y_Val += static_cast<NReal>(distr(random));
    }
    if (Y_MinMax)
    {
        Y_Val = Y_Val > Y_Max ? Y_Max : Y_Val < Y_Min ? Y_Min : Y_Val;
    }

    Y_valueQueue.pop_front();
    Y_valueQueue.Enqueue(Y_Val);

    return Y_Val;
}

double ARX_Model::CalcFunction(std::vector<double> arguments, NQueue<double> queue)
{
    double value = 0.0;
    for (int i = 0; i < arguments.size(); i++) {
        value += arguments[i] * queue.peek(queue.Size() - i - 1);
    }
    return value;
}

void ARX_Model::SetU_Limits(bool state, double min_, double max_)
{
    U_MinMax = state;
    U_Min = min_;
    U_Max = max_ < U_Min ? U_Min : max_;
}
void ARX_Model::SetY_Limits(bool state, double min_, double max_)
{
    Y_MinMax = state;
    Y_Min = min_;
    Y_Max = max_ < Y_Min ? Y_Min : max_;
}
void ARX_Model::SetAParams(std::vector<double> a_)
{
    if (aParams.size() > a_.size())
    {
        int diff = static_cast<int>(aParams.size() - a_.size());
        for (int i = 0; i < diff; i++) Y_valueQueue.pop_front();
    }
    else if (aParams.size() < a_.size())
    {
        int diff = static_cast<int>(a_.size() - aParams.size());
        for (int i = 0; i < diff; i++)
            Y_valueQueue.Enqueue(0.0);
    }
    aParams = a_;
}

std::vector<double> ARX_Model::GetAParams()
{
    return aParams;
}
std::vector<double> ARX_Model::GetBParams()
{
    return bParams;
}

void ARX_Model::SetBParams(std::vector<double> b_)
{
    if (bParams.size() > b_.size())
    {
        int diff = static_cast<int>(bParams.size() - b_.size());
        for (int i = 0; i < diff; i++) U_valueQueue.pop_front();
    }
    else if (bParams.size() < b_.size())
    {
        int diff = static_cast<int>(b_.size() - bParams.size());
        for (int i = 0; i < diff; i++) U_valueQueue.Enqueue(0.0);
    }
    bParams = b_;
}
void ARX_Model::SetK(int k_)
{
    k_ = k_ < 1 ? 1 : k_;
    if (k_ > k)
    {
        int diff = k_ - k;
        for (int i = 0; i < diff; i++) k_delayQueue.Enqueue(0.0);
    }
    else if (k_ < k)
    {
        int diff = k - k_;
        for (int i = 0; i < diff; i++) k_delayQueue.pop_front();
    }
    k = k_;

}
void ARX_Model::SetZ(double z_)
{
    z = z_ >= 0 ? z_ : fabs(z_);
    if (z > 0)
    {
        distr = std::normal_distribution<double>(0.0, z);
        random = std::mt19937(std::random_device{}());
    }
}

double ARX_Model::GetYValue()
{
    return Y_valueQueue.peek(Y_valueQueue.Size()-1);
}

std::tuple<double,double,bool> ARX_Model::getULimits(){
    return std::tuple<double,double,bool>(U_Min,U_Max,U_MinMax);
}
std::tuple<double,double,bool> ARX_Model::getYLimits(){
    return std::tuple<double,double,bool>(Y_Min,Y_Max,Y_MinMax);
}
int ARX_Model::getK(){return k;}
double ARX_Model::getZ(){return z;}
void ARX_Model::ResetQueue(){
    aParams.clear();
    bParams.clear();
    Y_valueQueue.Clear();
    U_valueQueue.Clear();
    k_delayQueue.Enqueue(0.0);
}

std::tuple<double,double> ARX_Model::GetDistr(){
    return std::tuple<double,double>(distr.mean(),distr.stddev());
}
double ARX_Model::getLastU(){
    return lastSavedU;
}

std::array<int,2> ARX_Model::ParamsCounter(){
    std::array<int,2> counters;
    counters[0]= aParams.size();
    counters[1]=bParams.size();
    return counters;
}
