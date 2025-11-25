#include "MeanRevertingProcess.h"

MeanRevertingProcess::MeanRevertingProcess(double speed, double target, double vol, WienerProcess& wn)
    : StochasticProcess(0, vol, wn),
      reversion_speed(speed),
      target_level(target)
{
}

long double MeanRevertingProcess::update(long double current_price, double dt)
{
    
    double current_drift = reversion_speed * (target_level - current_price);

    
    double change = (current_drift * dt) + (getDiffusion() * wiener.getIncrement(dt));

    return current_price + change;
}