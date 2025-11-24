#include "WienerProcess.h"

WienerProcess::WienerProcess() : 
    rng_engine(std::random_device{}()),
    normal_dist(0.0,1.0)
{
    
}

double WienerProcess::getIncrement(double dt)
{
    return std::sqrt(dt)* normal_dist(rng_engine);
}
