#include "BatesProcess.h"

BatesProcess::BatesProcess(double drift, double start_vol, 
                           WienerProcess& price_engine, WienerProcess& vol_engine,
                           double kappa, double theta, double xi,
                           double jump_intensity, double jump_mean, double jump_vol)
    : HestonProcess(drift, start_vol, price_engine, vol_engine, kappa, theta, xi),
      lambda(jump_intensity),
      jump_mu(jump_mean),
      jump_sigma(jump_vol),
      jump_rng(std::random_device{}()),
      jump_check_dist(0.0, 1.0),
      jump_size_dist(jump_mean, jump_vol)
{
}

long double BatesProcess::update(long double current_price, double dt)
{
    long double new_price = HestonProcess::update(current_price, dt);

    if (jump_check_dist(jump_rng) < lambda * dt)
    {
        double jump_magnitude = jump_size_dist(jump_rng);
        new_price += new_price * jump_magnitude;
    }

    return new_price;
}

void BatesProcess::setRegime(double intensity, double mean, double vol, double vol_of_vol)
{
    lambda = intensity;
    jump_mu = mean;
    jump_sigma = vol;
    
    
    this->xi = vol_of_vol; 
    
    
    jump_size_dist = std::normal_distribution<double>(jump_mu, jump_sigma);
}