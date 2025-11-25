#ifndef JUMP_DIFFUSION_PROCESS_H 
#define JUMP_DIFFUSION_PROCESS_H

#include "StochasticProcess.h"
#include <random>


class JumpDiffusionProcess : public StochasticProcess {
public:
    
    JumpDiffusionProcess(double dr, double df, WienerProcess& wn, 
                         double intensity, double j_mean, double j_vol);

    
    long double update(long double current_price, double dt) override;

private:
    double jump_intensity; 
    double jump_mean;      
    double jump_vol;       
    
    
    std::mt19937 jump_rng; 
    std::normal_distribution<double> jump_size_dist;
    std::uniform_real_distribution<double> jump_check_dist;
};

#endif //JUMP_DIFFUSION_PROCESS_H