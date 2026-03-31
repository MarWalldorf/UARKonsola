#include "Generator.h"


double Generator::GenSIN() {
    return A * sin((static_cast<double>((i++ % T) / static_cast<double>(T))) * 2 * M_PI) + S;
}
double Generator::GenSQR() {
    if ((i++ % T) < p * T)
    {
        return A + S;
    }
    else
    {
        return S;
    }
}
double Generator::Generate(){
    if(generator_type){
        return GenSQR();
    }else{
        return GenSIN();
    }
}
void Generator::setParameters(int T_) {
    T = T_;
}
void Generator::setAmplitude(double A_) {
    A = A_;
}
void Generator::setShift(double S_) {
    S = S_;
}
void Generator::setPulseWidth(double p_) {
    p = p_;
}
void Generator::resetCounter() {
    i = 0;
}
void Generator::SetGeneratorType(bool type_) {
    generator_type = type_;
}
bool Generator::GetGeneratorType() {
    return generator_type;
}
std::tuple<int,double,double,double> Generator::GetGeneratorParams(){
    return std::tuple<int,double,double,double>(T,A,S,p);
}
