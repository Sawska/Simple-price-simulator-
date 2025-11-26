#ifndef SIMULATION_APP_H
#define SIMULATION_APP_H

#include <SDL2/SDL.h>
#include <vector>
#include "StochasticProcess.h"
#include "BatesProcess.h"
#include "MeanRevertingProcess.h"

struct Candle {
    double open;
    double high;
    double low;
    double close;
};

class SimulationApp {
public:
    SimulationApp(int width, int height);
    ~SimulationApp();
    void run();

private:
    void initSDL();
    void handleEvents();
    void updatePhysics();
    void render();
    void cleanup();

    double map_range(double value, double in_min, double in_max, double out_min, double out_max);
    double calculate_lambda(double sentiment);

    void drawGrid(double min_p, double max_p);
    void drawPriceLabel(int x, int y, double price);
    void drawDigit(int x, int y, char digit);

    int screenHeight;
    int screenWidth;
    bool isRunning;
    bool isPaused; 

    float candleWidth; 
    int scrollOffset;  

    SDL_Window* window;
    SDL_Renderer* renderer;

    WienerProcess price_noise;
    WienerProcess vol_noise;
    WienerProcess news_noise;
    
    BatesProcess bates;
    MeanRevertingProcess sentiment_process;

    std::vector<Candle> candles;
    std::vector<double> price_history;
    Candle current_candle;       
    int tick_counter;            
    double current_sentiment;
};

#endif // SIMULATION_APP_H