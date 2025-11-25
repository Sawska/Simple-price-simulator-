#ifndef MEAN_REVERTING_PROCESS_H
#define MEAN_REVERTING_PROCESS_H

#include "StochasticProcess.h"

class MeanRevertingProcess : public StochasticProcess {
public:
    
    MeanRevertingProcess(double speed, double target, double vol, WienerProcess& wn);

    long double update(long double current_price, double dt) override;

private:
    double reversion_speed;
    double target_level;
};

#endif