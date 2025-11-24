#include "StochasticProcess.h"

StochasticProcess::StochasticProcess(double dr, double df,WienerProcess& wn) : 
drift(dr), diffusion(df), wiener(wn)
{

}

long double StochasticProcess::update(long double current_price,double dt)
{
long double change = current_price * drift * dt + diffusion * current_price * wiener.getIncrement(dt);
return current_price + change;
}
