#ifndef BATES_PROCESS_H
#define BATES_PROCESS_H

#include "HestonProcess.h"
#include <random>

class BatesProcess : public HestonProcess {
public:
    BatesProcess(double drift, double start_vol, 
                 WienerProcess& price_engine, WienerProcess& vol_engine,
                 double kappa, double theta, double xi,
                 double jump_intensity, double jump_mean, double jump_vol);

    long double update(long double current_price, double dt) override;

    // NEW: Add this function declaration
    void setRegime(double intensity, double mean, double vol, double vol_of_vol);

private:
    double lambda;      
    double jump_mu;     
    double jump_sigma;  

    std::mt19937 jump_rng;
    std::uniform_real_distribution<double> jump_check_dist;
    std::normal_distribution<double> jump_size_dist;
};

#endif