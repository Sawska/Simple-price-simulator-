#include "SimulationApp.h"
#include <iostream>
#include <algorithm> 
#include <cmath>
#include <string>
#include <cstdio> 

const int TICKS_PER_CANDLE = 100; 
const int RIGHT_MARGIN = 70;

SimulationApp::SimulationApp(int width, int height)
    : screenWidth(width), screenHeight(height), isRunning(false) ,
      window(nullptr), renderer(nullptr),
      bates(0.05, 0.20, price_noise, vol_noise, 2.0, 0.20, 0.50, 0.1, -0.10, 0.05),
      sentiment_process(0.8, 0.5, 0.25, news_noise),
      current_sentiment(0.5),
      tick_counter(0),
      candleWidth(10.0f),
      scrollOffset(0),
      isPaused(false)
{
    initSDL();
    current_candle = {100.0, 100.0, 100.0, 100.0};
    
    
    for(int i=0; i<screenWidth; ++i) {
        price_history.push_back(100.0);
    }
}

SimulationApp::~SimulationApp() { cleanup(); }

void SimulationApp::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    window = SDL_CreateWindow("Binance Style Simulator", 
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (!window) exit(1);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) exit(1);
    isRunning = true;
}

void SimulationApp::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) isRunning = false;
        
        if (e.type == SDL_MOUSEWHEEL) {
            if (e.wheel.y > 0) candleWidth += 2.0f;
            if (e.wheel.y < 0) candleWidth -= 2.0f;
            if (candleWidth < 2.0f) candleWidth = 2.0f;
            if (candleWidth > 50.0f) candleWidth = 50.0f;
        }
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_LEFT && isPaused) scrollOffset += 1;
            if (e.key.keysym.sym == SDLK_RIGHT && isPaused) {
                scrollOffset -= 1;
                if (scrollOffset < 0) scrollOffset = 0;
            }
            if (e.key.keysym.sym == SDLK_SPACE) isPaused = !isPaused;
        }
    }
}

void SimulationApp::updatePhysics() {
    if (isPaused) return; 

    current_sentiment = sentiment_process.update(current_sentiment, 1.0/50.0);
    if(current_sentiment > 1.0) current_sentiment = 1.0;
    if(current_sentiment < -1.0) current_sentiment = -1.0;

    double new_lambda = calculate_lambda(current_sentiment);
    double new_xi = (current_sentiment < 0) ? 1.50 : 0.30; 
    double new_jump_mean = (current_sentiment < 0) ? -0.40 : 0.05; 
    bates.setRegime(new_lambda, new_jump_mean, 0.10, new_xi);

    for(int k=0; k<10; ++k) {
        double price = bates.update(current_candle.close, 1.0/252.0);

        if (price > current_candle.high) current_candle.high = price;
        if (price < current_candle.low) current_candle.low = price;
        current_candle.close = price;

        tick_counter++;

        if (tick_counter >= TICKS_PER_CANDLE) {
            candles.push_back(current_candle);
            if (scrollOffset > 0) scrollOffset++;
            
            current_candle.open = price;
            current_candle.high = price;
            current_candle.low = price;
            current_candle.close = price;
            tick_counter = 0;
        }
    }
}

void SimulationApp::render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); 
    SDL_RenderClear(renderer);

    if (candles.empty()) {
        SDL_RenderPresent(renderer);
        return;
    }

    
    int chartWidth = screenWidth - RIGHT_MARGIN;
    int max_visible_candles = chartWidth / (int)candleWidth + 2;
    int total_candles = candles.size();
    
    int start_index = total_candles - max_visible_candles - scrollOffset;
    if (start_index < 0) start_index = 0;
    int end_index = total_candles - scrollOffset;
    if (end_index > total_candles) end_index = total_candles;

    
    double min_p = 999999999.0;
    double max_p = -999999999.0;

    for (int i = start_index; i < end_index; ++i) {
        if (candles[i].low < min_p) min_p = candles[i].low;
        if (candles[i].high > max_p) max_p = candles[i].high;
    }
    if (scrollOffset == 0) {
        if (current_candle.low < min_p) min_p = current_candle.low;
        if (current_candle.high > max_p) max_p = current_candle.high;
    }

    double padding = (max_p - min_p) * 0.1;
    if (padding == 0) padding = 1.0; // Prevent divide by zero on flat line
    min_p -= padding;
    max_p += padding;

    // 4. Draw Mesh Grid & Axis Labels (Binance Style)
    drawGrid(min_p, max_p);

    // 5. Draw Candles
    for (int i = start_index; i < end_index; ++i) {
        Candle& c = candles[i];
        int x_pos = (i - start_index) * candleWidth;

        int y_open = (int)map_range(c.open, min_p, max_p, screenHeight - 50, 50);
        int y_close = (int)map_range(c.close, min_p, max_p, screenHeight - 50, 50);
        int y_high = (int)map_range(c.high, min_p, max_p, screenHeight - 50, 50);
        int y_low = (int)map_range(c.low, min_p, max_p, screenHeight - 50, 50);

        bool isBullish = (c.close >= c.open);
        if (isBullish) SDL_SetRenderDrawColor(renderer, 0, 255, 128, 255); 
        else SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);

        int center_x = x_pos + (candleWidth / 2);
        SDL_RenderDrawLine(renderer, center_x, y_high, center_x, y_low);

        int rect_y = isBullish ? y_close : y_open;
        int rect_h = abs(y_close - y_open);
        if (rect_h < 1) rect_h = 1;

        SDL_Rect body = { x_pos + 1, rect_y, (int)candleWidth - 2, rect_h };
        SDL_RenderFillRect(renderer, &body);
    }

    // 6. Draw Live Candle
    if (scrollOffset == 0) {
        int x_pos = (end_index - start_index) * candleWidth;
        int y_open = (int)map_range(current_candle.open, min_p, max_p, screenHeight - 50, 50);
        int y_close = (int)map_range(current_candle.close, min_p, max_p, screenHeight - 50, 50);
        int y_high = (int)map_range(current_candle.high, min_p, max_p, screenHeight - 50, 50);
        int y_low = (int)map_range(current_candle.low, min_p, max_p, screenHeight - 50, 50);

        bool isBullish = (current_candle.close >= current_candle.open);
        if (isBullish) SDL_SetRenderDrawColor(renderer, 0, 255, 128, 255); 
        else SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);

        int center_x = x_pos + (candleWidth / 2);
        SDL_RenderDrawLine(renderer, center_x, y_high, center_x, y_low);
        
        int rect_y = isBullish ? y_close : y_open;
        int rect_h = abs(y_close - y_open);
        if (rect_h < 1) rect_h = 1;
        SDL_Rect body = { x_pos + 1, rect_y, (int)candleWidth - 2, rect_h };
        SDL_RenderFillRect(renderer, &body);
        
        // Draw current price line
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 100); // Faint white line
        SDL_RenderDrawLine(renderer, 0, y_close, screenWidth - RIGHT_MARGIN, y_close);
        
        // Highlight current price label
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawPriceLabel(screenWidth - RIGHT_MARGIN + 5, y_close - 4, current_candle.close);
    }

    // 7. Draw UI (Sentiment)
    int bar_height = 20;
    SDL_Rect sentiment_bar = {0, screenHeight - bar_height, screenWidth, bar_height};
    if (current_sentiment >= 0) SDL_SetRenderDrawColor(renderer, 0, (int)(current_sentiment * 255), 0, 255);
    else SDL_SetRenderDrawColor(renderer, (int)(std::abs(current_sentiment) * 255), 0, 0, 255);
    SDL_RenderFillRect(renderer, &sentiment_bar);

    if (isPaused) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_Rect pause_box = {10, 10, 20, 60};
        SDL_RenderFillRect(renderer, &pause_box);
        SDL_Rect pause_box2 = {40, 10, 20, 60};
        SDL_RenderFillRect(renderer, &pause_box2);
    }

    SDL_RenderPresent(renderer);
}



void SimulationApp::drawGrid(double min_p, double max_p) {
    double range = max_p - min_p;
    // Calculate a "nice" step size (e.g., 10, 5, 1, 0.5)
    double step = std::pow(10, std::floor(std::log10(range)));
    if (range / step < 4) step /= 2.0;
    if (range / step > 10) step *= 2.0;

    // Find first grid line
    double start = std::ceil(min_p / step) * step;

    // Draw horizontal lines
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255); // Dark Gray
    
    for (double p = start; p < max_p; p += step) {
        int y = (int)map_range(p, min_p, max_p, screenHeight - 50, 50);
        
        // 1. Draw Line
        SDL_RenderDrawLine(renderer, 0, y, screenWidth - RIGHT_MARGIN, y);
        
        // 2. Draw Label on Right Axis
        drawPriceLabel(screenWidth - RIGHT_MARGIN + 5, y - 4, p);
    }
    
    // Draw Vertical Axis Line
    SDL_RenderDrawLine(renderer, screenWidth - RIGHT_MARGIN, 0, screenWidth - RIGHT_MARGIN, screenHeight);
}

void SimulationApp::drawPriceLabel(int x, int y, double price) {
    char buffer[16];
    sprintf(buffer, "%.2f", price); // Format to 2 decimals
    
    int cursor_x = x;
    for(int i=0; buffer[i] != '\0'; ++i) {
        drawDigit(cursor_x, y, buffer[i]);
        cursor_x += 7; // Spacing between digits
    }
}

// A mini "Pixel Font" renderer
void SimulationApp::drawDigit(int x, int y, char digit) {
    // Simple 5x7 pixel font logic
    // Using SDL_RenderDrawLine for segments
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255); // Light Gray Text

    switch(digit) {
        case '0': 
            SDL_RenderDrawLine(renderer, x, y, x+4, y);     // Top
            SDL_RenderDrawLine(renderer, x, y+6, x+4, y+6); // Bottom
            SDL_RenderDrawLine(renderer, x, y, x, y+6);     // Left
            SDL_RenderDrawLine(renderer, x+4, y, x+4, y+6); // Right
            break;
        case '1':
            SDL_RenderDrawLine(renderer, x+2, y, x+2, y+6);
            break;
        case '2':
            SDL_RenderDrawLine(renderer, x, y, x+4, y);     // Top
            SDL_RenderDrawLine(renderer, x+4, y, x+4, y+3); // Right Top
            SDL_RenderDrawLine(renderer, x, y+3, x+4, y+3); // Middle
            SDL_RenderDrawLine(renderer, x, y+3, x, y+6);   // Left Bottom
            SDL_RenderDrawLine(renderer, x, y+6, x+4, y+6); // Bottom
            break;
        case '3':
            SDL_RenderDrawLine(renderer, x, y, x+4, y);     // Top
            SDL_RenderDrawLine(renderer, x, y+3, x+4, y+3); // Middle
            SDL_RenderDrawLine(renderer, x, y+6, x+4, y+6); // Bottom
            SDL_RenderDrawLine(renderer, x+4, y, x+4, y+6); // Right
            break;
        case '4':
            SDL_RenderDrawLine(renderer, x, y, x, y+3);     // Left Top
            SDL_RenderDrawLine(renderer, x, y+3, x+4, y+3); // Middle
            SDL_RenderDrawLine(renderer, x+4, y, x+4, y+6); // Right
            break;
        case '5':
            SDL_RenderDrawLine(renderer, x, y, x+4, y);     // Top
            SDL_RenderDrawLine(renderer, x, y, x, y+3);     // Left Top
            SDL_RenderDrawLine(renderer, x, y+3, x+4, y+3); // Middle
            SDL_RenderDrawLine(renderer, x+4, y+3, x+4, y+6); // Right Bottom
            SDL_RenderDrawLine(renderer, x, y+6, x+4, y+6); // Bottom
            break;
        case '6':
            SDL_RenderDrawLine(renderer, x, y, x+4, y);     // Top
            SDL_RenderDrawLine(renderer, x, y, x, y+6);     // Left
            SDL_RenderDrawLine(renderer, x, y+3, x+4, y+3); // Middle
            SDL_RenderDrawLine(renderer, x, y+6, x+4, y+6); // Bottom
            SDL_RenderDrawLine(renderer, x+4, y+3, x+4, y+6); // Right Bottom
            break;
        case '7':
            SDL_RenderDrawLine(renderer, x, y, x+4, y);     // Top
            SDL_RenderDrawLine(renderer, x+4, y, x+4, y+6); // Right
            break;
        case '8':
            SDL_RenderDrawLine(renderer, x, y, x+4, y);     // Top
            SDL_RenderDrawLine(renderer, x, y+3, x+4, y+3); // Middle
            SDL_RenderDrawLine(renderer, x, y+6, x+4, y+6); // Bottom
            SDL_RenderDrawLine(renderer, x, y, x, y+6);     // Left
            SDL_RenderDrawLine(renderer, x+4, y, x+4, y+6); // Right
            break;
        case '9':
            SDL_RenderDrawLine(renderer, x, y, x+4, y);     // Top
            SDL_RenderDrawLine(renderer, x, y+3, x+4, y+3); // Middle
            SDL_RenderDrawLine(renderer, x, y, x, y+3);     // Left Top
            SDL_RenderDrawLine(renderer, x+4, y, x+4, y+6); // Right
            break;
        case '.':
            SDL_RenderDrawPoint(renderer, x+2, y+6);
            break;
    }
}

void SimulationApp::cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

double SimulationApp::map_range(double value, double in_min, double in_max, double out_min, double out_max) {
    return out_min + (value - in_min) * (out_max - out_min) / (in_max - in_min);
}

double SimulationApp::calculate_lambda(double sentiment) {
    if (sentiment > 0) return 0.1; 
    return std::pow(sentiment, 4) * 200.0; 
}

void SimulationApp::run() {
    while (isRunning) {
        handleEvents();
        updatePhysics();
        render();
        SDL_Delay(16);
    }
}