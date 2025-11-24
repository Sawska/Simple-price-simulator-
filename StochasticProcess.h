#ifndef STOCHASTIC_PROCESS_H
#define STOCHASTIC_PROCESS_H

#include "WienerProcess.h"

class StochasticProcess {
public:
    StochasticProcess(double dr,double df,WienerProcess& wn);
    long double update(long double current_price,double dt);
private:
    double drift;
    double diffusion;
    WienerProcess& wiener;
};

#endif //STOCHASTIC_PROCESS_H