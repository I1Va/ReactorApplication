#ifndef CLOCK_WIDGET_H
#define CLOCK_WIDGET_H

#include "gm_primitives.hpp"
#include "MyGUI.h"


class ClockWindow : public Window {
    static constexpr const double CLOCK_LAYOUT_SHARE = 0.75;
    static constexpr const double CLOCK_MARK_LEN = 2;
    static constexpr const int FONT_SIZE = 10;

    static constexpr const char TOP_NUMBER_TEXT_[] = "0";
    static constexpr const char RIGHT_NUMBER_TEXT_[] = "15";
    static constexpr const char BOTTOM_NUMBER_TEXT_[] = "30";
    static constexpr const char LEFT_NUMBER_TEXT_[] = "45";

    static constexpr const SDL_Color TEXT_COLOR = {0, 0, 0, 255};

    int currentTimeMS_ = 0;
    int length_ = 0;
    int clockRadius_ = 0;
    gm_dot<int, 2> clockCenter_ = {};

    TTF_Font* font_ = nullptr;

    SDL_Texture* topNumber_      = nullptr;
    SDL_Texture* bottomNumber_   = nullptr;
    SDL_Texture* leftNumber_     = nullptr;
    SDL_Texture* rightNumber_    = nullptr;
    SDL_Texture* dialTexture_    = nullptr;

private:
    void drawClockMarks(SDL_Renderer* renderer) {
        assert(renderer);
    
        gm_vector<double, 2> mark = {1, 0};

        for (int second = 0; second < 60; second++) {
            

            int x1 = length_ / 2 + mark.get_x() * (clockRadius_ - CLOCK_MARK_LEN / 2);
            int y1 = length_ / 2 + mark.get_y() * (clockRadius_ - CLOCK_MARK_LEN / 2);

            int x2 = x1 + mark.get_x() * CLOCK_MARK_LEN;
            int y2 = y1 + mark.get_y() * CLOCK_MARK_LEN;

            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

            mark = mark.rotate(M_PI * 2 / 60);
        }
    }

    void drawNumberTexture(SDL_Texture* texture, const gm_dot<int, 2> pos, SDL_Renderer* renderer) {
        assert(texture);
        assert(renderer);

        SDL_Rect dstRect = {pos.x, pos.y, 0, 0};
        SDL_QueryTexture(texture, NULL, NULL, &dstRect.w, &dstRect.h);

        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    
    }
    void drawClockNumbers(SDL_Renderer* renderer) {
        drawNumberTexture(topNumber_, {clockCenter_.x, clockCenter_.y - clockRadius_ - FONT_SIZE * 2}, renderer);
        drawNumberTexture(bottomNumber_, {clockCenter_.x, clockCenter_.y + clockRadius_ + FONT_SIZE}, renderer);
        drawNumberTexture(leftNumber_, {clockCenter_.x - clockRadius_ - FONT_SIZE * 2, clockCenter_.y}, renderer);
        drawNumberTexture(rightNumber_, {clockCenter_.x + clockRadius_ + FONT_SIZE, clockCenter_.y}, renderer);
    }

    void initNumberTextures(SDL_Renderer* renderer) {
        topNumber_      = createFontTexture(font_, TOP_NUMBER_TEXT_, FONT_SIZE, TEXT_COLOR, renderer);
        bottomNumber_   = createFontTexture(font_, BOTTOM_NUMBER_TEXT_, FONT_SIZE, TEXT_COLOR, renderer);
        leftNumber_     = createFontTexture(font_, LEFT_NUMBER_TEXT_, FONT_SIZE, TEXT_COLOR, renderer);
        rightNumber_    = createFontTexture(font_, RIGHT_NUMBER_TEXT_, FONT_SIZE, TEXT_COLOR, renderer);

        assert(topNumber_);
        assert(bottomNumber_);
        assert(leftNumber_);
        assert(rightNumber_);
    }

    void initDialTexture(SDL_Renderer* renderer) {
        dialTexture_ = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            length_,
            length_
        );

        SDL_Texture* prevTarget = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, dialTexture_);


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, DEFAULT_WINDOW_COLOR.r, DEFAULT_WINDOW_COLOR.g, DEFAULT_WINDOW_COLOR.b, DEFAULT_WINDOW_COLOR.a); 
        SDL_Rect full = {0, 0, rect_.w, rect_.h};
        SDL_RenderFillRect(renderer, &full);
        
    
        filledCircleColor(renderer, length_ / 2, length_ / 2, clockRadius_, SDL2gfxColorToUint32(WHITE_SDL_COLOR));
        circleColor(renderer, length_ / 2, length_ / 2, clockRadius_, SDL2gfxColorToUint32(BLACK_SDL_COLOR));

        drawClockMarks(renderer);

        initNumberTextures(renderer);
        drawClockNumbers(renderer);

        SDL_SetRenderTarget(renderer, prevTarget);
    }   

    void drawClockHand(SDL_Renderer* renderer) {
        gm_vector<double, 2> clockHand = {0, -1};
        clockHand = clockHand * clockRadius_;
        clockHand = clockHand.rotate((double) currentTimeMS_ / (60 * SEC_TO_MS) * 2 * M_PI);
    
        double x1 = length_ / 2;
        double y1 = length_ / 2;
        double x2 = x1 + clockHand.get_x();
        double y2 = y1 + clockHand.get_y();

        SDL_SetRenderDrawColor(renderer, BLACK_SDL_COLOR.r, BLACK_SDL_COLOR.g, BLACK_SDL_COLOR.b, BLACK_SDL_COLOR.a);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }

public:
    ClockWindow(int length, const char fontPath[], Widget *parent=nullptr): Window(length, length, parent), length_(length) {
        clockRadius_ = length / 2 * CLOCK_LAYOUT_SHARE;
        clockCenter_ = {length / 2, length / 2};
    
        font_ = TTF_OpenFont(fontPath, FONT_SIZE); 
        if (!font_) {
            SDL_Log("TTF_OpenFont: %s", TTF_GetError());
            assert(0);
        }
        
    }

    ~ClockWindow() override { TTF_CloseFont(font_); }


    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);
    
        static bool initDialTextureFlag = true;
        
        if (initDialTextureFlag) {
            initDialTexture(renderer);
            initDialTextureFlag = false;
        }

        SDL_Rect dst = {0, 0, length_, length_};
        SDL_RenderCopy(renderer, dialTexture_, NULL, &dst);
    
        drawClockHand(renderer);
    }

    void updateClock(double deltaMS) {
        currentTimeMS_ += deltaMS;
        if (currentTimeMS_ >= 60 * SEC_TO_MS) currentTimeMS_ = 0;
        setRerenderFlag();
    }
};



#endif // CLOCK_WIDGET_H