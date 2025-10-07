#ifndef PLOTS_H
#define PLOTS_H

#include <vector>
#include <deque>
#include <limits>

#include "MyGUI.h"

const int RECORDER_BORDER_SIZE = 10;
const int RECORDER_LINE_THICKNESS = 1;
const int RECORDER_AXEMARK_THICKNESS = 2;
const int RECORDER_DOT_SIZE = 1;

struct CordPoint {
    double x, y;
    int type;   
};

class PlotModel {
    std::vector<CordPoint> points_;

public:
    PlotModel() {}

    void addPoint(double x, double y, int type) { points_.push_back({x, y, type}); }
    const std::vector<CordPoint> points() const { return points_; }
};

struct RecordPoint {
    double y;
    bool state;
    unsigned int type;
};

class RecorderModel {
    int pixelWidth_ = 0;
    int pixelHeight_ = 0;

    bool curPointState = true;
    double curScaleY_ = 1;

    std::deque<RecordPoint> points_;

    bool needsRecalc_ = true;

private:
    void setRecalculatePointsFlag() { needsRecalc_ = true; }

public:
    RecorderModel(double pixelWidth, double pixelHeight) : pixelWidth_(pixelWidth), pixelHeight_(pixelHeight) {}

    void addPoint(double y, unsigned int type) { 
        if (points_.size() == 0 && std::abs(y) > std::numeric_limits<double>::epsilon())
            curScaleY_ = pixelHeight_ / y;
    
        if (points_.size() > pixelWidth_) {
            bool fstPointState = points_.front().state;
            while (points_.size()) {
                bool curPointState = points_.front().state;
                if (curPointState != fstPointState) break;
                points_.pop_front();
            }
        }
        
        points_.push_back({y, curPointState, type});

        double pixelY = y * curScaleY_;
        if (pixelY > pixelHeight_) curScaleY_ = pixelHeight_ / y;
    }

    void endRecord() { curPointState = !curPointState; }

    double scaleY() const { return curScaleY_; }
    std::deque<RecordPoint> points() const { return points_; }
};

class RecorderWidget : public Widget {
    RecorderModel recorder_;

private:
    void drawVerticalAxe(SDL_Renderer* renderer, double scaleY) {
        assert(renderer);
        
        SDL_Rect rect = {0, 0, RECORDER_LINE_THICKNESS, rect_.h};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderFillRect(renderer, &rect);

        // for (double y = rect_.h - scaleY; y >= 0; y -= scaleY) {
        //     SDL_Rect mark = {0, (int) y, RECORDER_AXEMARK_THICKNESS * 2, RECORDER_AXEMARK_THICKNESS};
        //     SDL_RenderFillRect(renderer, &mark);
        // }
    }

public:
    RecorderWidget(int width, int height, Widget *parent=nullptr): Widget(width, height, parent), recorder_(width, height) {}

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);
    
        SDL_Rect widgetRect = {0, 0, rect_.w, rect_.h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &widgetRect);

        drawVerticalAxe(renderer, recorder_.scaleY());

        for (int i = 0; i < recorder_.points().size(); i++) {
            RecordPoint point = recorder_.points()[i];
            SDL_Color pointColor = Uint32ToSDL2gfxColor(point.type);
            
            double x = i;
            double y = -(point.y * recorder_.scaleY()) + rect_.h;

            SDL_SetRenderDrawColor(renderer, pointColor.r, pointColor.g, pointColor.b, pointColor.a);
            filledCircleColor(renderer, x, y, RECORDER_DOT_SIZE, point.type);
        }
    }

    void addPoint(double y, SDL_Color color) { recorder_.addPoint(y, SDL2gfxColorToUint32(color)); }
    void endRecord() { recorder_.endRecord(); }
};

class RecorderWindow : public Window {
    RecorderWidget *recorder_;

public:
    RecorderWindow(int height, int width, Widget *parent=nullptr): Window(height, width, parent) {
        recorder_ = new RecorderWidget(width - 2 * WINDOW_BORDER_SIZE, height - 2 * WINDOW_BORDER_SIZE, this);
        addWidget(WINDOW_BORDER_SIZE, WINDOW_BORDER_SIZE, recorder_);
    }

    void addPoint(double y, SDL_Color color) { 
        recorder_->addPoint(y, color); 
        recorder_->setRerenderFlag();
    }

    void endRecord() { recorder_->endRecord(); }
};


#endif // PLOTS_H