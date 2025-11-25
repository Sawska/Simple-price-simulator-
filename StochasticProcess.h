#ifndef STOCHASTIC_PROCESS_H
#define STOCHASTIC_PROCESS_H

#include "WienerProcess.h"

class StochasticProcess {
public:
    StochasticProcess(double dr,double df,WienerProcess& wn);
    virtual long double update(long double current_price,double dt);
    virtual ~StochasticProcess() = default;
    double getDiffusion();
    double getDrift();
private:
    double drift;
    double diffusion; 
protected:
WienerProcess& wiener;
};

#endif //STOCHASTIC_PROCESS_H