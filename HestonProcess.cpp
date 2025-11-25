#include "HestonProcess.h"
#include <cmath>

HestonProcess::HestonProcess(double dr, double start_vol, WienerProcess& wn1, WienerProcess& wn2,
                             double vol_speed, double vol_target, double vol_of_vol)
    : StochasticProcess(dr, 0, wn1), // Pass 0 for diffusion, we handle it internally
      current_volatility(start_vol),
      vol_wiener(wn2),
      kappa(vol_speed),
      theta(vol_target),
      xi(vol_of_vol)
{
}

long double HestonProcess::update(long double current_price, double dt)
{

    double dW_vol = vol_wiener.getIncrement(dt);
    
    double vol_drift = kappa * (theta - current_volatility) * dt;
    double vol_diff  = xi * std::sqrt(std::max(0.0, current_volatility)) * dW_vol;
    
    current_volatility += vol_drift + vol_diff;


    current_volatility = std::max(0.001, current_volatility);


    double dW_price = wiener.getIncrement(dt);
    

    double price_drift = current_price * getDrift() * dt;
    double price_diff  = current_price * current_volatility * dW_price;

    return current_price + price_drift + price_diff;
}