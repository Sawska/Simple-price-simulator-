#include "StochasticProcess.h"
#include "BatesProcess.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <random> 

using namespace std::chrono_literals;

int main()
{
    
    WienerProcess price_noise;
    WienerProcess vol_noise;

    BatesProcess market(0.05, 0.20, price_noise, vol_noise, 
                        2.0, 0.20, 0.30, 
                        0.5, -0.01, 0.02);

    std::mt19937 regime_rng(std::random_device{}());
    std::uniform_real_distribution<double> regime_dist(0.0, 1.0);
    
    bool is_chaos_regime = false;
    long double price = 100.00;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "--- STOCHASTIC MARKET SIMULATION ---" << std::endl;
    std::cout << "Regime Switching: Probabilistic" << std::endl;

    while(true)
    {

        double probability_roll = regime_dist(regime_rng);

        if (!is_chaos_regime) 
        {
            if (probability_roll < 0.01) 
            {
                is_chaos_regime = true;
                std::cout << "\n>>> REGIME SHIFT: HIGH VOLATILITY DETECTED <<<" << std::endl;
                

                market.setRegime(40.0, -0.15, 0.10, 0.90);
            }
        } 
        else 
        {
            if (probability_roll < 0.04) 
            {
                is_chaos_regime = false;
                std::cout << "\n>>> REGIME SHIFT: MARKET STABILIZING <<<" << std::endl;
                
                market.setRegime(0.5, -0.01, 0.02, 0.30);
            }
        }

        long double old_price = price;
        price = market.update(price, 1.0/252.0);

        if (is_chaos_regime) {
            std::cout << "Price: $" << std::setw(8) << price << " [VOLATILE]";
        } else {
            std::cout << "Price: $" << std::setw(8) << price << " [NORMAL]  ";
        }

        if (price < old_price * 0.90) std::cout << " <--- CRASH!";
        if (price > old_price * 1.10) std::cout << " <--- SPIKE!";
        
        std::cout << std::endl;
        
        std::this_thread::sleep_for(50ms);
    }

    return 0;
}