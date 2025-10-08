#ifndef PLOTS_H
#define PLOTS_H

#include <vector>
#include <deque>
#include <limits>

#include "MyGUI.h"
#include "ScrollBar.h"


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
    bool reScalingMode_ = false;
    int pixelWidth_ = 0;
    int pixelHeight_ = 0;

    bool curPointState = true;
    double fixedYScale = 1;

    std::deque<RecordPoint> points_;

    bool needsRecalc_ = true;

private:
    void setRecalculatePointsFlag() { needsRecalc_ = true; }

public:
    RecorderModel(double pixelWidth, double pixelHeight, double startScale) : 
        pixelWidth_(pixelWidth), pixelHeight_(pixelHeight), fixedYScale(startScale) {}

    void addPoint(double y, unsigned int type) { 
        if (reScalingMode_ && points_.size() == 0 && std::abs(y) > std::numeric_limits<double>::epsilon())
            fixedYScale = pixelHeight_ / y;
    
        if (points_.size() > pixelWidth_) {
            bool fstPointState = points_.front().state;
            while (points_.size()) {
                bool curPointState = points_.front().state;
                if (curPointState != fstPointState) break;
                points_.pop_front();
            }
        }
        
        points_.push_back({y, curPointState, type});

        double pixelY = y * fixedYScale;
        if (pixelY > pixelHeight_) fixedYScale = pixelHeight_ / y;
    }

    void endRecord() { curPointState = !curPointState; }

    double yScale() const { return fixedYScale; }
    std::deque<RecordPoint> points() const { return points_; }
};

class RecorderWidget : public Widget {
    RecorderModel recorder_;
    double xScale_ = 1;
    double yScale_ = 1;
    bool reScalingMode_ = false;

private:
    void drawVerticalAxe(SDL_Renderer* renderer, double yScale) {
        assert(renderer);
        
        SDL_Rect rect = {0, 0, RECORDER_LINE_THICKNESS, rect_.h};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderFillRect(renderer, &rect);
    }

public:
    RecorderWidget(int width, int height, double startScale=1, bool reScalingMode=false, Widget *parent=nullptr): 
        Widget(width, height, parent), recorder_(width, height, startScale), reScalingMode_(reScalingMode) {}

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);
    
        SDL_Rect widgetRect = {0, 0, rect_.w, rect_.h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &widgetRect);

        drawVerticalAxe(renderer, recorder_.yScale());

        for (int i = 0; i < recorder_.points().size(); i++) {
            RecordPoint point = recorder_.points()[i];
            SDL_Color pointColor = Uint32ToSDL2gfxColor(point.type);
            
            double xScale = xScale_;
            double yScale = (reScalingMode_ ? recorder_.yScale() : yScale_);
            

            double x = i * xScale;
            double y = -(point.y * yScale) + rect_.h;

            SDL_SetRenderDrawColor(renderer, pointColor.r, pointColor.g, pointColor.b, pointColor.a);
            filledCircleColor(renderer, x, y, RECORDER_DOT_SIZE, point.type);
        }
    }

    void setXScale(double scale) { 
        xScale_ = scale; 
        setRerenderFlag(); 
    }

    void setYScale(double scale) { 
        yScale_ = scale; 
        setRerenderFlag(); 
    }

    void addPoint(double y, SDL_Color color) { recorder_.addPoint(y, SDL2gfxColorToUint32(color)); }
    void endRecord() { recorder_.endRecord(); }
};

class RecorderWindow : public Window {
    RecorderWidget *recorder_ = nullptr;

public:
    RecorderWindow(int height, int width, double startScale=1, bool reScalingMode=false, Widget *parent=nullptr): Window(height, width, parent) {
        recorder_ = new RecorderWidget(width - 2 * WINDOW_BORDER_SIZE, height - 2 * WINDOW_BORDER_SIZE, startScale, reScalingMode, this);
        addWidget(WINDOW_BORDER_SIZE, WINDOW_BORDER_SIZE, recorder_);
    }

    void addPoint(double y, SDL_Color color) { 
        recorder_->addPoint(y, color); 
        recorder_->setRerenderFlag();
    }

    void endRecord() { recorder_->endRecord(); }
};

class ScrollRecorderWindow : public Window {
    static constexpr const double SCROLL_BAR_LAYOUT_SHARE = 0.2;
    static constexpr const double X_ZOOM_COEF = 5;
    static constexpr const double Y_ZOOM_COEF = 20;
    int scrollBarLen_ = 0;

    ScrollBar *xScaleScrollBar_ = nullptr;
    ScrollBar *yScaleScrollBar_ = nullptr;

    RecorderWidget *recorder_ = nullptr;

    double startScaleY_ = 1;
    double startScaleX_ = 1;

// ScrollBar(int width,  int height, std::function<void(double)> onScroll, bool isHorizontal = true, Widget *parent=nullptr):
public:
    ScrollRecorderWindow(int height, int width, double startScale=1, bool reScalingMode=false, Widget *parent=nullptr): 
        Window(height, width, parent), startScaleY_(startScale) {
        scrollBarLen_ = width * SCROLL_BAR_LAYOUT_SHARE - 2 * RECORDER_BORDER_SIZE;
    
        xScaleScrollBar_ = new ScrollBar(width - scrollBarLen_ - 2 * RECORDER_BORDER_SIZE, scrollBarLen_, 
            [this](double percent) { recorder_->setXScale(startScaleX_ * (1 + percent)); }, true, this);
        
        yScaleScrollBar_ = new ScrollBar(scrollBarLen_, height - scrollBarLen_ - 2 * RECORDER_BORDER_SIZE, 
            [this](double percent) { recorder_->setYScale(startScaleY_ * percent); }, false, this);

        addWidget(scrollBarLen_ + RECORDER_BORDER_SIZE, RECORDER_BORDER_SIZE, xScaleScrollBar_);
        addWidget(RECORDER_BORDER_SIZE, scrollBarLen_ + RECORDER_BORDER_SIZE, yScaleScrollBar_);

        recorder_ = new RecorderWidget(width - scrollBarLen_ - 2 * RECORDER_BORDER_SIZE, height - scrollBarLen_ - 2 * RECORDER_BORDER_SIZE, startScale, reScalingMode, this);
        addWidget(RECORDER_BORDER_SIZE + scrollBarLen_, RECORDER_BORDER_SIZE + scrollBarLen_, recorder_);
    }

    void addPoint(double y, SDL_Color color) { 
        recorder_->addPoint(y, color); 
        recorder_->setRerenderFlag();
    }

    void endRecord() { recorder_->endRecord(); }
};


#endif // PLOTS_H