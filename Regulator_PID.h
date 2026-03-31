#pragma once
#include <tuple>


class Regulator_PID
{
private:
    void SaveIntegralValue(double);
    double Kp = 0.0, Ti = 0.0, Td = 0.0;
    double l_Kp_val, l_Ti_val, l_Td_val;
    double last_e = 0.0;
    double Sum_Int = 0.0;
    bool integral_type = false;
public:
    //Regulator_PID(FeedBack_loop* loop_,double Kp_,double Ti_,double Td_,bool I_type_);
    Regulator_PID(double Kp_ = 0, double Ti_ = 0, double Td_ = 0, bool I_type_ = 0);
    ~Regulator_PID() {};
    double Calc_Propocional(double e);
    double Calc_Integral(double e);// tryb=0-stala przed calka, 1-stala pod calka
    double Calc_Derivative(double e);
    double Calc_PID(double e);
    void set_Kp(double Kp_);
    void set_Ti(double Ti_);
    void set_Td(double Td_);
    void set_IType(bool I_type_);
    void Clear_Integral();
    void Clear_Derivative();
    double symuluj(double input_);
    double getLastKpValue();
    double getLastTiValue();
    double getLastTdValue();
    std::tuple<double,double,double,bool> GetRegParams();
    double getKp();
    double getTi();
    double getTd();
};

