#include "Regulator_PID.h"

Regulator_PID::Regulator_PID(double Kp_, double Ti_, double Td_, bool I_type_)
{
    Kp = Kp_;
    Ti = Ti_;
    Td = Td_;
    integral_type = I_type_;
}

double Regulator_PID::Calc_Propocional(double e)
{
    l_Kp_val = Kp*e;
    return l_Kp_val;
}

double Regulator_PID::Calc_Integral(double e)
{

    double integral_value = 0.0;
    if (this->Ti != 0.0)
    {
        SaveIntegralValue(e);
        if (this->integral_type)
        {
            l_Ti_val = Sum_Int;
            return l_Ti_val;
        }
        else
        {
            l_Ti_val = Sum_Int / Ti;
            return l_Ti_val;
        }
    }
    l_Ti_val = integral_value;
    return integral_value;
}
double Regulator_PID::Calc_Derivative(double e)
{
    double derivative_value = Td * (e - last_e);
    last_e = e;
    l_Td_val = derivative_value;
    return derivative_value;
}
void Regulator_PID::SaveIntegralValue(double e)
{

    if (integral_type) {
        Sum_Int += e / Ti;
    }
    else
    {
        Sum_Int += e;
    }


}
double Regulator_PID::Calc_PID(double e)
{
    return Calc_Propocional(e) + Calc_Integral(e) + Calc_Derivative(e);
}
void Regulator_PID::set_Kp(double Kp_)
{
    Kp = Kp_;
}
void Regulator_PID::set_Ti(double Ti_)
{
    Ti = Ti_;
}
void Regulator_PID::set_Td(double Td_)
{
    Td = Td_;
}
void Regulator_PID::Clear_Integral() {
    Sum_Int = 0.0;
}
void Regulator_PID::Clear_Derivative(){
    last_e = 0;
}
void Regulator_PID::set_IType(bool I_type_)
{
    if (I_type_ != integral_type)
    {
        if (I_type_ == false)
        {
            Sum_Int = Sum_Int * Ti;
        }
        else
        {
            Sum_Int = Sum_Int / Ti;
        }

    }
    integral_type = I_type_;

}
double Regulator_PID::symuluj(double e)
{
    return Calc_PID(e);
}

double Regulator_PID::getLastKpValue(){return l_Kp_val;};
double Regulator_PID::getLastTiValue(){return l_Ti_val;};
double Regulator_PID::getLastTdValue(){return l_Td_val;};

std::tuple<double,double,double,bool> Regulator_PID::GetRegParams(){
    return std::tuple<double,double,double,bool>(Kp,Td,Ti,integral_type);
}
double Regulator_PID::getKp(){return Kp;}
double Regulator_PID::getTi(){return Ti;}
double Regulator_PID::getTd(){return Td;}
