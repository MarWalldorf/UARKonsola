#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <tuple>
#include "NQueue.h"


class Generator
{
private:
    int T;
    double A;
    double S;
    double p;
    long long i = 0;
    bool generator_type = false; // false - sin, true - square
public:
    Generator() {};
    ~Generator() {};
    double GenSIN();
    double GenSQR();
    void setParameters(int T_);
    NQueue<double> getQueue();
    void setAmplitude(double A_);
    void setShift(double S_);
    void setPulseWidth(double p_);
    void resetCounter();
    void SetGeneratorType(bool type_);
    bool GetGeneratorType();
    double Generate();
    std::tuple<int,double,double,double> GetGeneratorParams();
};

