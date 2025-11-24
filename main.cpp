#include "StochasticProcess.h"
#include <iostream>

int main()
{

    WienerProcess wp1 = WienerProcess();
    StochasticProcess sp1 = StochasticProcess(0.1,0.2,wp1);

    long double c = 1;
    while(true)
    {
        c = sp1.update(c,1.0/152);
        std::cout << c << std::endl;
    }


}