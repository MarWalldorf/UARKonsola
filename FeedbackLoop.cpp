#include "FeedbackLoop.h"
FeedbackLoop::FeedbackLoop(ARX_Model model, Regulator_PID pid,Generator gen)
    : regulator(pid), arx_model(model), generator(gen)
{};

double FeedbackLoop::ObliczSygnalWejsciowy()
{
    double wartosc_wejscia = 0;
    if (getGenerator().GetGeneratorType())
    {
        wartosc_wejscia = generator.GenSQR();
    }
    else
    {
        wartosc_wejscia = generator.GenSIN();
    }
    wartosc_wejscia = wartosc_wejscia - lastY;
    return wartosc_wejscia;
}

double FeedbackLoop::UruchomSymulacje(double e_)
{
    double e = e_ - lastY;
    lastE = e;
    double wartosc_wyjscia_regulatora = regulator.Calc_PID(e);
    double y = arx_model.GrindingFunction(wartosc_wyjscia_regulatora);
    lastU = arx_model.getLastU();
    lastY = y;
    return y;
}

std::array<double,7> FeedbackLoop::Symuluj(){
    double wz = wzadane;
    if(useGen){
        wz = generator.Generate();
    }


    double out = UruchomSymulacje(wz);
    return {wz,out,lastU,regulator.getLastKpValue(),regulator.getLastTiValue(), regulator.getLastTdValue(),lastE};
}

ARX_Model& FeedbackLoop::getARXModel()
{
    return arx_model;
}
Regulator_PID& FeedbackLoop::getRegulator()
{
    return regulator;
}
Generator& FeedbackLoop::getGenerator()
{
    return generator;
}
void FeedbackLoop::setUseGen(bool state){
    useGen = state;
}
void FeedbackLoop::setWZadane(double wz){
    wzadane=wz;
}
double FeedbackLoop::GetWz(){
    return wzadane;
}
void FeedbackLoop::Reset(){
    lastY = 0;
    lastE = 0;
    lastU = 0;
    wzadane = 0;
    arx_model.ResetQueue();
    regulator.Clear_Integral();
    regulator.Clear_Derivative();
    regulator.set_Kp(0);
    regulator.set_Ti(0);
    regulator.set_Td(0);

    // Reset Generatora
    generator.resetCounter();
    generator.setAmplitude(0);
    generator.setShift(0);
    generator.setParameters(0);
    generator.setPulseWidth(0.0);
}


