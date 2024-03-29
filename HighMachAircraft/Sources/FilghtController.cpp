#include "FlightController.h"
#include "AircraftModel.h"
#include <iostream>
#include <fstream>
#include "Airframe.h"
#include <cmath>

FlightController::FlightController()
{
    pAirObj = 0;
    guidance = new Guidance;
    FlightControllerModels.push_back(guidance);
}

FlightController::FlightController(AircraftModel* pObj)
{
    pAirObj = pObj;
    guidance = new Guidance(pObj);
    FlightControllerModels.push_back(guidance);
}

FlightController::~FlightController()
{
    delete guidance;
}

void FlightController::Initial()
{
    guidance->Initial();
}

void FlightController::UpdateState(double timeCur, AircraftModel* pAirObject)
{
    for (size_t i = 0; i < FlightControllerModels.size(); i++)
    {
        FlightControllerModels[i]->UpdateState(timeCur, pAirObject);
    }
}

void FlightController::UpdateOutput(double timeCur, AircraftModel* pAirObject)
{
    for (size_t i = 0; i < FlightControllerModels.size(); i++)
    {
        FlightControllerModels[i]->UpdateOutput(timeCur, pAirObject);
    }
}

void FlightController::UpdateDerivate(double timeCur, AircraftModel* pAirObject)
{
    for (size_t i = 0; i < FlightControllerModels.size(); i++)
    {
        FlightControllerModels[i]->UpdateDerivate(timeCur, pAirObject);
    }
}

void FlightController::getFileOutputItemName(vector<string>& FileOutItemName)
{
    guidance->getFileOutputItemName(FileOutItemName);
}

ostream& operator<<(ostream& os, const FlightController& pObj)
{
    os << *(pObj.guidance);
    return os;
}

Guidance::Guidance()
{
    pAirObj = 0;
    GuidancePhase = 1;
    firstGuidance = new FirstGuidance;
    secondGuidance = new SecondGuidance;
    thirdGuidance = new ThirdGuidance;
    fouthGuidance = new FouthGuidance;

    GuidanceModels.push_back(firstGuidance);
    GuidanceModels.push_back(secondGuidance);
    GuidanceModels.push_back(thirdGuidance);
    GuidanceModels.push_back(fouthGuidance);

    command.assign(3, 0);
}

Guidance::Guidance(AircraftModel* pObj)
{
    pAirObj = pObj;
    GuidancePhase = 1;
    firstGuidance = new FirstGuidance(pObj);
    secondGuidance = new SecondGuidance(pObj);
    thirdGuidance = new ThirdGuidance(pObj);
    fouthGuidance = new FouthGuidance(pObj);

    GuidanceModels.push_back(firstGuidance);
    GuidanceModels.push_back(secondGuidance);
    GuidanceModels.push_back(thirdGuidance);
    GuidanceModels.push_back(fouthGuidance);

    command.assign(3, 0);
}

Guidance::~Guidance()
{
    delete firstGuidance;
    delete secondGuidance;
    delete thirdGuidance;
    delete fouthGuidance;
}
void Guidance::Initial()
{
    firstGuidance->Initial();
    secondGuidance->Initial();
    thirdGuidance->Initial();
    fouthGuidance->Initial();
}

void Guidance::UpdateState(double timeCur, AircraftModel* pAirObject)
{
    UpdateGuidancePhase(timeCur, pAirObject);
    switch (GuidancePhase)
    {
    case 1:
        firstGuidance->UpdateState(timeCur, pAirObject);
        //command = firstGuidance->getCommand();
        break;
    case 2:
        secondGuidance->UpdateState(timeCur, pAirObject);
        //command = secondGuidance->getCommand();
        break;
    case 3:
        thirdGuidance->UpdateState(timeCur, pAirObject);
        //command = thirdGuidance->getCommand();
        break;
    case 4:
        fouthGuidance->UpdateState(timeCur, pAirObject);
        //command = thirdGuidance->getCommand();
        break;
    default:
        break;
    }
}

void Guidance::UpdateOutput(double timeCur, AircraftModel* pAirObject)
{
    switch (GuidancePhase)
    {
    case 1:
        firstGuidance->UpdateOutput(timeCur, pAirObject);
        command = firstGuidance->getCommand();
        break;
    case 2:
        secondGuidance->UpdateOutput(timeCur, pAirObject);
        command = secondGuidance->getCommand();
        break;
    case 3:
        thirdGuidance->UpdateOutput(timeCur, pAirObject);
        command = thirdGuidance->getCommand();
        break;
    case 4:
        fouthGuidance->UpdateOutput(timeCur, pAirObject);
        command = fouthGuidance->getCommand();
        break;
    default:
        break;
    }
}

void Guidance::UpdateDerivate(double timeCur, AircraftModel* pAirObject)
{
    switch (GuidancePhase)
    {
    case 1:
        firstGuidance->UpdateDerivate(timeCur, pAirObject);
        //command = firstGuidance->getCommand();
        break;
    case 2:
        secondGuidance->UpdateDerivate(timeCur, pAirObject);
        //command = secondGuidance->getCommand();
        break;
    case 3:
        thirdGuidance->UpdateDerivate(timeCur, pAirObject);
        //command = thirdGuidance->getCommand();
        break;
    case 4:
        fouthGuidance->UpdateDerivate(timeCur, pAirObject);
        //command = thirdGuidance->getCommand();
        break;
    default:
        break;
    }
}

void Guidance::UpdateGuidancePhase(double timeCur, AircraftModel* pAirObject)
{
    double H = pAirObject->getSysState()->Position[1];
    double m = pAirObject->getSysState()->m;
    if (timeCur < 11.0)
    {
        GuidancePhase = 1;
    }
    else
    {
        if (GuidancePhase == 1)
        {
            GuidancePhase = 2;
            cout << "爬升段\t" << timeCur << endl;
        }
        if (GuidancePhase == 2)
        {
            if (H > 25000)
            {
                pAirObject->time_2 = timeCur;
                GuidancePhase = 3;
                pAirObject->e_theta = 0;
                cout << "巡航段\t" << timeCur << endl;
            }
        }
        if (GuidancePhase == 3)
        {
            if (m < 3301)
            {
                GuidancePhase = 4;
                pAirObject->e_theta = 0;
                pAirObject->time_4 = timeCur;
                pAirObject->getSysState()->setm(3300);
                cout << "返航段\t" << timeCur << endl;
            }

        }
    }
}

void Guidance::getFileOutputItemName(vector<string>& FileOutItemName)
{
    FileOutItemName.push_back("GuidancePhase");
    FileOutItemName.push_back("AlphaCommand");
    FileOutItemName.push_back("SigmaCommand");
    FileOutItemName.push_back("RatioCommand");
}

ostream& operator<<(ostream& os, const Guidance& pObj)
{
    os << pObj.GuidancePhase << "\t" << pObj.command[0] << "\t" << pObj.command[1] << "\t" << pObj.command[2];
    return os;
}

FirstGuidance::FirstGuidance()
{
    pAirObj = 0;
    command.assign(3, 0);
}

FirstGuidance::FirstGuidance(AircraftModel* pObj)
{
    pAirObj = pObj;
    command.assign(3, 0);
}

FirstGuidance::~FirstGuidance()
{
}
void FirstGuidance::Initial()
{
}

void FirstGuidance::UpdateState(double timeCur, AircraftModel* pAirObject)
{
}

void FirstGuidance::UpdateOutput(double timeCur, AircraftModel* pAirObject)
{
    command[0] = 0;
    command[1] = 0;
    command[2] = 0;
}

void FirstGuidance::UpdateDerivate(double timeCur, AircraftModel* pAirObject)
{
}

SecondGuidance::SecondGuidance()
{
    pAirObj = 0;
    command.assign(3, 0);
}

SecondGuidance::SecondGuidance(AircraftModel* pObj)
{
    pAirObj = pObj;
    command.assign(3, 0);
}

SecondGuidance::~SecondGuidance()
{
}
void SecondGuidance::Initial()
{
}

double SecondGuidance::calcuAlpha(double timeCur, AircraftModel* pAirObject)
{

    double thrust = pAirObject->getAirframe()->getPropulsion()->getThrustTotal();
    double mass = pAirObject->getAirframe()->getPropulsion()->getMass();
    double g = pAirObject->getAirframe()->getDynamics()->getG();
    double Q = pAirObject->getAirframe()->getDynamics()->getQ();
    double S = pAirObject->getAirframe()->getAeroData()->getSref();
    double ma = pAirObject->getAirframe()->getDynamics()->getMa();
    vec Vel = pAirObject->getAirframe()->getDynamics()->getVelocity();
    double theta = 0;
    if (timeCur < 40)
    {
        theta = 10.0 / r2d;
    }
    else if (timeCur < 70)
    {
        theta = 25.0 / r2d;
    }
    else if (timeCur < 235)
    {
        theta = 6.3 / r2d;
    }
    else if (timeCur < 312)
    {
        theta = 6 / r2d;
    }
    else
    {
        theta = 5 / r2d;
    }

    double V = Vel[0];
    double x0 = 0.03;
    double x1 = 0.05;
    double Cl;
    double Cl_alpha;

    while (fabs(x1 - x0) / fabs(x1) > 1e-12)
    {
        x0 = x1;
        Cl = pAirObject->getAirframe()->getAeroData()->getCl(x0, pAirObject);
        Cl_alpha = pAirObject->getAirframe()->getAeroData()->getCl_alpha(x0, pAirObject);
        x1 = x0 - ((thrust * sin(x0) + Q * S * Cl - mass * g * cos(Vel[1])) / mass / V + 1.414 * (Vel[1] - theta) / 5 + pAirObject->e_theta / 25) / ((thrust * cos(x0) + Q * S * Cl_alpha) / mass / V);
    }

    if (x1 >= 25.0 / 57.3)
    {
        x1 = 25.0 / 57.3;
    }
    if (x1 <= -25.0 / 57.3)
    {
        x1 = -25.0 / 57.3;
    }

    return x1;
}

void SecondGuidance::UpdateState(double timeCur, AircraftModel* pAirObject)
{
}

void SecondGuidance::UpdateOutput(double timeCur, AircraftModel* pAirObject)
{
    double alpha = 0;
    if (timeCur < 15.006)
    {
        command[0] = 9.0 / 180 * pi;
        command[1] = 0;
        command[2] = 0;
    }
    else
    {
        alpha = calcuAlpha(timeCur, pAirObject);
        command[0] = alpha;
        command[1] = 0;
        command[2] = 0;
    }
}

void SecondGuidance::UpdateDerivate(double timeCur, AircraftModel* pAirObject)
{
}

ThirdGuidance::ThirdGuidance()
{
    pAirObj = 0;
    command.assign(3, 0);
}

ThirdGuidance::ThirdGuidance(AircraftModel* pObj)
{
    pAirObj = pObj;
    command.assign(3, 0);
}

ThirdGuidance::~ThirdGuidance()
{
}
void ThirdGuidance::Initial()
{
}

void ThirdGuidance::UpdateState(double timeCur, AircraftModel* pAirObject)
{
}

double ThirdGuidance::calcuAlpha(double timeCur, AircraftModel* pAirObject)
{

    double thrust = pAirObject->getAirframe()->getPropulsion()->getThrustTotal();
    double mass = pAirObject->getAirframe()->getPropulsion()->getMass();
    double g = pAirObject->getAirframe()->getDynamics()->getG();
    double Q = pAirObject->getAirframe()->getDynamics()->getQ();
    double S = pAirObject->getAirframe()->getAeroData()->getSref();
    double ma = pAirObject->getAirframe()->getDynamics()->getMa();
    vec Vel = pAirObject->getAirframe()->getDynamics()->getVelocity();
    double theta = 0;
    // if (timeCur < 40)
    // {
    //     theta = 10.0 / r2d;
    // }
    // else if (timeCur < 70)
    // {
    //     theta = 25.0 / r2d;
    // }
    // else if (timeCur < 235)
    // {
    //     theta = 6.3 / r2d;
    // }
    // else if (timeCur < 312)
    // {
    //     theta = 4.7 / r2d;
    // }
    // else
    // {
    //     theta = 3.5 / r2d;
    // }

    if (pAirObject->heightFlag == false)
    {
        theta = ((0.18 - 5) / 70 * (timeCur - pAirObject->time_2) + 5) / r2d;
    }
    else
    {
        theta = 0.1 / r2d;
    }

    double V = Vel[0];
    double x0 = 0.03;
    double x1 = 0.05;
    double Cl;
    double Cl_alpha;

    while (fabs(x1 - x0) / fabs(x1) > 1e-12)
    {
        x0 = x1;
        Cl = pAirObject->getAirframe()->getAeroData()->getCl(x0, pAirObject);
        Cl_alpha = pAirObject->getAirframe()->getAeroData()->getCl_alpha(x0, pAirObject);
        x1 = x0 - ((thrust * sin(x0) + Q * S * Cl - mass * g * cos(Vel[1])) / mass / V + 1.414 * (Vel[1] - theta) / 5 + pAirObject->e_theta / 25) / ((thrust * cos(x0) + Q * S * Cl_alpha) / mass / V);
    }

    if (x1 >= 25.0 / 57.3)
    {
        x1 = 25.0 / 57.3;
    }
    if (x1 <= -25.0 / 57.3)
    {
        x1 = -25.0 / 57.3;
    }

    return x1;
}

void ThirdGuidance::UpdateOutput(double timeCur, AircraftModel* pAirObject)
{
    double h = pAirObject->getAirframe()->getDynamics()->getH();
    double V = pAirObject->getAirframe()->getDynamics()->getVelScalar();
    vec com;
    if (pAirObject->heightFlag == false)
    {
        if (h < 28500.0)
        {
            command[0] = calcuAlpha(timeCur, pAirObject);
            command[1] = 0;
            command[2] = 0;
        }
        else
        {
            pAirObject->heightFlag = true;
        }
    }
    else if (pAirObject->velFlag == false)
    {
        if (V < 1780)
        {
            command[0] = calcuAlpha(timeCur, pAirObject);
        }
        else
        {
            pAirObject->velFlag = true;
        }

    }
    else
    {
        com = calcuControl(timeCur, pAirObject);
        //command[0] = 1.4/57.3;
        command[0] = com[0];
        command[1] = com[1];
        command[2] = com[2];
    }
}

void ThirdGuidance::UpdateDerivate(double timeCur, AircraftModel* pAirObject)
{
}

int Sign(double x)
{
    int a;
    if (x >= 0)
    {
        a = 1;
    }
    else
    {
        a = -1;
    }
    return a;
}

vec ThirdGuidance::calcuControl(double timeCur, AircraftModel* pAirObject)
{

    double thrust = pAirObject->getAirframe()->getPropulsion()->getThrustTotal();
    double mass = pAirObject->getAirframe()->getPropulsion()->getMass();
    double g = pAirObject->getAirframe()->getDynamics()->getG();
    double Q = pAirObject->getAirframe()->getDynamics()->getQ();
    double rho = pAirObject->getAirframe()->getDynamics()->getrho();
    double S = pAirObject->getAirframe()->getAeroData()->getSref();
    double ma = pAirObject->getAirframe()->getDynamics()->getMa();
    vec Vel = pAirObject->getAirframe()->getDynamics()->getVelocity();
    double h = pAirObject->getAirframe()->getDynamics()->getH();
    double Vr = 1800.0;
    double hr = 30000.0;
    double V = Vel[0];
    double theta = Vel[1];
    double Psi = Vel[2];
    double x0 = 0.01;
    double x1 = 0.02;
    double Cl;
    double Cl_alpha;
    double Cd;
    double Cd_alpha;

    double n = 0;
    double time_manu;
    if (timeCur > 1400 && timeCur < 1800)
    {
        n = 5 * cos(2 * pi / 200.0 * (timeCur - 1400));
        //n = 20;
    }
    if (timeCur > 1800 && Psi > 1e-6)
    {
        n = 5 * cos(2 * pi / 200.0 * (timeCur - 1400));
        //n = 20;
    }
    double omega_h = 1.0 / 10;
    double omega_v = 1.0 / 10;

    double a1 = cos(theta) / mass;
    double a2 = 1.0 / mass;
    double a3 = 1.0 / mass / V / cos(theta);
    double c1 = g * cos(theta) * cos(theta) - 2 * 0.707 * omega_h * V * sin(theta) - (h - 30000.0) * omega_h * omega_h + (1.414 * (Vel[0] - Vr) * omega_v + pAirObject->e_v * omega_v * omega_v) * sin(theta);
    double c2 = g * sin(theta) - 1.414 * (Vel[0] - Vr) * omega_v - pAirObject->e_v * omega_v * omega_v;
    double c3 = n / V;
    double sigma = atan(c3 * a1 / c1 / a3);
    vec control_out(3, 0.0);
    // if (timeCur < 700)
    // {
    //     while (fabs(x1 - x0) / fabs(x1) > 1e-12)
    //     {
    //         x0 = x1;
    //         Cd = pAirObject->getAirframe()->getAeroData()->getCd(x0, pAirObject);
    //         Cd_alpha = pAirObject->getAirframe()->getAeroData()->getCd_alpha(x0, pAirObject);
    //         x1 = x0 - ((thrust * cos(x0) - Q * S * Cd - mass * g * sin(Vel[1])) / mass + 1.414 * (Vel[0] - Vr) / 300.0 + pAirObject->e_v / 300 / 300.0) / ((-thrust * sin(x0) - Q * S * Cd_alpha) / mass);
    //         //x1 = x0 - (thrust * sin(x0) + Q * S * Cl - mass * g * cos(Vel[1])) / (thrust * cos(x0) + Q * S * Cl_alpha);
    //     }

    //     if (x1 >= 25.0 / 57.3)
    //     {
    //         x1 = 25.0 / 57.3;
    //     }
    //     if (x1 <= -25.0 / 57.3)
    //     {
    //         x1 = -25.0 / 57.3;
    //     }
    //     control_out[0] = x1;
    // }
    // x0 = 0.01;
    // x1 = 0.02;
    if (n > 10)
    {
        x1 = 0.2;
    }


    while (fabs(x1 - x0) / fabs(x1) > 1e-12)
    {
        x0 = x1;
        Cd = pAirObject->getAirframe()->getAeroData()->getCd(x0, pAirObject);
        Cd_alpha = pAirObject->getAirframe()->getAeroData()->getCd_alpha(x0, pAirObject);
        Cl = pAirObject->getAirframe()->getAeroData()->getCl(x0, pAirObject);
        Cl_alpha = pAirObject->getAirframe()->getAeroData()->getCl_alpha(x0, pAirObject);
        x1 = x0 - (a1 * cos(sigma) * (c2 - a2 * Q * S * Cd) * tan(x0) - (c1 - Q * S * Cl * a1 * cos(sigma)) * a2) / (a1 * cos(sigma) * (-a2 * Q * S * Cd_alpha) * tan(x0) + a1 * cos(sigma) * (c2 - a2 * Q * S * Cd) * (1.0 / cos(x0) / cos(x0)) + Q * S * Cl_alpha * a1 * a2 * cos(sigma));
        //x1 = x0 - (thrust * sin(x0) + Q * S * Cl - mass * g * cos(Vel[1])) / (thrust * cos(x0) + Q * S * Cl_alpha);
    }
    if (x1 >= 45.0 / 57.3)
    {
        x1 = 45.0 / 57.3;
    }
    if (x1 <= -45.0 / 57.3)
    {
        x1 = -45.0 / 57.3;
    }
    Cd = pAirObject->getAirframe()->getAeroData()->getCd(x1, pAirObject);
    double T = (c2 + a2 * Q * S * Cd) / a2 / cos(x1);
    double ratio;
    double temp_a = -600;
    double temp_b = -68 * ma * ma + 454 * ma + 826 + 600;
    double temp_c = T / 9.8 / (0.55 * V * rho / 14.9);
    double ratio_max = -temp_b / 2 / temp_a;
    if (ratio_max > 1.4)
    {
        ratio_max = 1.4;
    }
    if ((temp_a * ratio_max * ratio_max + temp_b * ratio_max) * 9.8 * 0.55 * V * rho / 14.9 < T)
    {
        ratio = ratio_max;
    }
    else
    {
        ratio = (-temp_b + sqrt(temp_b * temp_b - 4 * temp_a * (-temp_c))) / 2 / temp_a;
    }

    if (ratio > 1.4)
    {
        ratio = 1.4;
    }
    if (ratio < 0.5)
    {
        ratio = 0.5;
    }

    control_out[0] = x1;
    control_out[1] = sigma;
    control_out[2] = ratio;
    return control_out;
}


FouthGuidance::FouthGuidance()
{
    pAirObj = 0;
    command.assign(3, 0);
}

FouthGuidance::FouthGuidance(AircraftModel* pObj)
{
    pAirObj = pObj;
    command.assign(3, 0);
}

FouthGuidance::~FouthGuidance()
{
}
void FouthGuidance::Initial()
{
}

double FouthGuidance::calcuAlpha(double timeCur, AircraftModel* pAirObject)
{

    double thrust = pAirObject->getAirframe()->getPropulsion()->getThrustTotal();
    double mass = pAirObject->getAirframe()->getPropulsion()->getMass();
    double g = pAirObject->getAirframe()->getDynamics()->getG();
    double Q = pAirObject->getAirframe()->getDynamics()->getQ();
    double S = pAirObject->getAirframe()->getAeroData()->getSref();
    double ma = pAirObject->getAirframe()->getDynamics()->getMa();
    vec Vel = pAirObject->getAirframe()->getDynamics()->getVelocity();
    double theta = 0;
    if (timeCur < pAirObject->time_4 + 370)
    {
        if (timeCur < pAirObject->time_4 + 100)
        {
            theta = (-1.5 - 0) / 150 * (timeCur - pAirObject->time_4) / r2d;
        }
        else
        {
            theta = -1.5 / r2d;
        }

    }
    else if (timeCur < pAirObject->time_4 + 500)
    {
        theta = -4.0 / r2d;
    }
    else if (timeCur < pAirObject->time_4 + 600)
    {
        theta = -7 / r2d;
    }
    else if (timeCur < pAirObject->time_4 + 1000)
    {
        theta = -8 / r2d;
    }
    else
    {
        theta = -2 / r2d;
    }

    double V = Vel[0];
    double x0 = 0.03;
    double x1 = 0.05;
    double Cl;
    double Cl_alpha;

    while (fabs(x1 - x0) / fabs(x1) > 1e-12)
    {
        x0 = x1;
        Cl = pAirObject->getAirframe()->getAeroData()->getCl(x0, pAirObject);
        Cl_alpha = pAirObject->getAirframe()->getAeroData()->getCl_alpha(x0, pAirObject);
        x1 = x0 - ((thrust * sin(x0) + Q * S * Cl - mass * g * cos(Vel[1])) / mass / V + 1.414 * (Vel[1] - theta) / 5 + pAirObject->e_theta / 25) / ((thrust * cos(x0) + Q * S * Cl_alpha) / mass / V);
    }

    if (x1 >= 45.0 / 57.3)
    {
        x1 = 45.0 / 57.3;
    }
    if (x1 <= -45.0 / 57.3)
    {
        x1 = -45.0 / 57.3;
    }

    return x1;
}

void FouthGuidance::UpdateState(double timeCur, AircraftModel* pAirObject)
{
}

void FouthGuidance::UpdateOutput(double timeCur, AircraftModel* pAirObject)
{
    double alpha = 0;

    alpha = calcuAlpha(timeCur, pAirObject);
    command[0] = alpha;
    command[1] = 0;
    command[2] = 0;

}

void FouthGuidance::UpdateDerivate(double timeCur, AircraftModel* pAirObject)
{
}