#ifndef HESTON_PROCESS_H
#define HESTON_PROCESS_H

#include "StochasticProcess.h"
#include <algorithm> 

class HestonProcess : public StochasticProcess {
public:
    HestonProcess(double drift, double start_vol, 
                  WienerProcess& price_engine, WienerProcess& vol_engine,
                  double kappa, double theta, double xi);

    long double update(long double current_price, double dt) override;

protected: 
    double current_volatility; 
    double kappa; 
    double theta; 
    double xi;    
    
    WienerProcess& vol_wiener; 
};

#endif