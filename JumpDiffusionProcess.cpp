#include "JumpDiffusionProcess.h"
#include <random>


JumpDiffusionProcess::JumpDiffusionProcess(double dr, double df, WienerProcess& wn, 
                                           double intensity, double j_mean, double j_vol)
    : StochasticProcess(dr, df, wn), 
      jump_intensity(intensity),
      jump_mean(j_mean),
      jump_vol(j_vol),
      jump_rng(std::random_device{}()), 
      jump_size_dist(j_mean, j_vol),    
      jump_check_dist(0.0, 1.0)         
{
}

long double JumpDiffusionProcess::update(long double current_price, double dt)
{
    long double new_price = StochasticProcess::update(current_price, dt);


    double roll = jump_check_dist(jump_rng);

    if (roll < jump_intensity * dt)
    {
        
        double jump_percentage = jump_size_dist(jump_rng);

        new_price += new_price * jump_percentage;
    }

    return new_price;
}