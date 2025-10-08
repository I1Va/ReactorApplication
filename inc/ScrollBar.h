#ifndef SCROL_BAR_H
#define SCROL_BAR_H

#include "gm_primitives.hpp"
#include "MyGUI.h"

const ButtonTexturePath scrollBarTopBtnPath    = {"images/scrollBar/topButton/unpressed.png", "images/scrollBar/topButton/pressed.png"};
const ButtonTexturePath scrollBarBottomBtnPath = {"images/scrollBar/bottomButton/unpressed.png", "images/scrollBar/bottomButton/pressed.png"};
const ButtonTexturePath scrollThumbBtnPath     = {"images/scrollBar/thumbButton/unpressed.png", "images/scrollBar/thumbButton/pressed.png"};

class ThumbButton : public Button {
    gm_dot<int, 2> accumulatedRel_ = {};
    bool replaced_ = false;

    SDL_Rect movingArea_ = {};

public:
    ThumbButton
    (
        int width, int height,
        SDL_Rect movingArea,
        const ButtonTexturePath texturePath,
        std::function<void()> onClickFunction=nullptr, Widget *parent=nullptr
    ): Button(width, height, texturePath.unpressed, texturePath.pressed, onClickFunction, parent),
       movingArea_(movingArea) {};

    bool onMouseMoveSelfAction(const MouseMotionEvent &event) {
        if (this == UIManager_->mouseActived() && event.button == SDL_BUTTON_LEFT) {
            accumulatedRel_ += event.rel;
            replaced_ = true;
            return true;
        }

        return false;
    }

    bool updateSelfAction() {
        if (replaced_) {
            rect_.x += accumulatedRel_.x;
            rect_.y += accumulatedRel_.y;

            rect_.x = std::clamp(rect_.x, movingArea_.x, movingArea_.x + movingArea_.w);
            rect_.y = std::clamp(rect_.y, movingArea_.y, movingArea_.y + movingArea_.h);
            
            accumulatedRel_ = {0, 0};
            replaced_ = false;
            if (parent_) parent_->invalidate();
            return true;
        }
        
        return false;
    }

};


class ScrollBar : public Window {
    static constexpr double BUTTON_LAYOUT_SHARE_ = 0.1; 

    std::function<void(double)> onScroll_;

    double percentage_ = 0; 
    bool isHorizontal_;

    gm_dot<int, 2> buttonSize_ = {};

    SDL_Rect thumbMovingArea_ = {};

    Button *bottomButton_   = nullptr;
    Button *topButton_      = nullptr;
    ThumbButton *thumbButton_    = nullptr;

private:
    gm_dot<int, 2> getThumbPos(double percentage) {
        percentage = std::clamp(percentage, 0.0, 100.0);
    
        double len = (isHorizontal_? rect_.w - 2 * buttonSize_.x : rect_.h - 2 * buttonSize_.y);
        double x = (isHorizontal_? buttonSize_.x + percentage * len : 0);
        double y = (!isHorizontal_? buttonSize_.y + percentage * len : 0);
        return {(int) x, (int) y};
    }

    void move(double deltaPercent) {
        const double newPercentage = std::clamp(percentage_ + deltaPercent, 0.0, 1.0);
        const double realDelta = newPercentage - percentage_;
        
        onScroll_(newPercentage);

        gm_dot<int, 2> thumbPos = getThumbPos(percentage_);
        int relX = (isHorizontal_ ? realDelta : 0) * rect_.w;
        int relY = (!isHorizontal_ ? realDelta : 0) * rect_.h;


        MouseMotionEvent event(thumbPos.x, thumbPos.y, SDL_BUTTON_LEFT, relX, relY);
        

        thumbButton_->onMouseMove(event);

        percentage_ = newPercentage;
    }


public:
    ScrollBar(int width,  int height, std::function<void(double)> onScroll, bool isHorizontal = true, Widget *parent=nullptr): 
        Window(width, height, parent), onScroll_(onScroll), isHorizontal_(isHorizontal) 
    {

        buttonSize_ = 
        {
            (int) (( isHorizontal ? BUTTON_LAYOUT_SHARE_ : 1) * rect_.w),
            (int) ((!isHorizontal ? BUTTON_LAYOUT_SHARE_ : 1) * rect_.h)
        };

        topButton_ = new Button(buttonSize_.x, buttonSize_.y, scrollBarTopBtnPath.unpressed, scrollBarTopBtnPath.pressed, nullptr, this);
        addWidget(( isHorizontal ? rect_.w - buttonSize_.x : 0), 0, topButton_);

        bottomButton_ = new Button(buttonSize_.x, buttonSize_.y, scrollBarBottomBtnPath.unpressed, scrollBarBottomBtnPath.pressed, nullptr, this);
        addWidget(0, (!isHorizontal ? rect_.h - buttonSize_.y: 0), bottomButton_);

        thumbMovingArea_.x = (isHorizontal ? buttonSize_.x : 0);
        thumbMovingArea_.y = (!isHorizontal ? buttonSize_.y : 0);
        thumbMovingArea_.w = (isHorizontal ? rect_.w - 3 * buttonSize_.x : 0);
        thumbMovingArea_.h = (!isHorizontal ? rect_.h - 3 * buttonSize_.y : 0);


        gm_dot<int, 2> thumbPos = getThumbPos(percentage_);
        
        thumbButton_ = new ThumbButton(buttonSize_.x, buttonSize_.y, thumbMovingArea_, scrollThumbBtnPath, nullptr, this);
        addWidget(thumbPos.x, thumbPos.y, thumbButton_);
        
    }

    virtual bool onMouseDownSelfAction(const MouseButtonEvent &event) override {
        // hitting a free area
        return true;
    }


    // virtual bool onMouseMoveSelfAction(const MouseMotionEvent &event) override {
    //     if (thumbButton_->()) {
    //     const double curThumbCoord = (
    //         isHorizontal_ 
    //       ? event.coord.x - rect_.GetLeftCorner().x 
    //       : event.coord.y - rect_.GetLeftCorner().y
    //     );
    //     const double maxThumbCoord = (
    //         isHorizontal_ 
    //       ? rect_.Width()  - thumbButton_->GetRect().Width()
    //       : rect_.Height() - thumbButton_->GetRect().Height()
    //     );

    //     const double curPercentage = curThumbCoord / maxThumbCoord;
    //     const double shiftPercent = curPercentage - percentage_;

    //     ERROR_HANDLE(Move(shiftPercent));

    //     return true;
    // }

    // return OnMouseDrag(event);
    // }
};


#endif // SCROL_BAR_H