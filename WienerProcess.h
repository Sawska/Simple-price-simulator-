#ifndef WIENER_PROCESS_H
#define WIENER_PROCESS_H

#include <random>

class WienerProcess {
private:
    std::mt19937 rng_engine;
    std::normal_distribution<double> normal_dist;

public:
    WienerProcess();
    double getIncrement(double dt);
};

#endif //WIENER_PROCESS_H