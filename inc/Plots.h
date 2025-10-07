#ifndef PLOTS_H
#define PLOTS_H

#include <vector>
#include <deque>

#include "MyGUI.h"


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
        double pixelY = y * curScaleY_;
        
        if (points_.size() > pixelWidth_) {
            bool fstPointState = points_.front().state;
            while (points_.size()) {
                bool curPointState = points_.front().state;
                if (curPointState != fstPointState) break;
                points_.pop_front();
            }
        }
    
        points_.push_back({pixelY, curPointState, type});
        
        if (pixelY * curScaleY_ > pixelHeight_) curScaleY_ = pixelHeight_ / pixelY;
    }

    void endRecord() { curPointState = !curPointState; }

    double scaleY() const { return curScaleY_; }
    std::deque<RecordPoint> points() const { return points_; }
};

class RecorderWidget : public Widget {
    RecorderModel recorder;

public:
    RecorderWidget(int width, int height, Widget *parent=nullptr): Widget(width, height, parent), recorder(width, height) {}

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);
    
        SDL_Rect widgetRect = {0, 0, rect_.w, rect_.h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &widgetRect);

        
        for (int i = 0; i < recorder.points().size(); i++) {
            RecordPoint point = recorder.points()[i];
            SDL_Color pointColor = Uint32ToSDL_Color(point.type);
        
            SDL_SetRenderDrawColor(renderer, pointColor.r, pointColor.g, pointColor.b, pointColor.a);
            SDL_RenderDrawPoint(renderer, i, point.y * recorder.scaleY());
        }
    }

    void addPoint(double y, SDL_Color color) { recorder.addPoint(y, SDLColorToUint32(color)); }
    void endRecord() { recorder.endRecord(); }
};


#endif // PLOTS_H