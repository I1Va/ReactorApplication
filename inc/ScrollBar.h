#ifndef SCROL_BAR_H
#define SCROL_BAR_H

#include "gm_primitives.hpp"
#include "MyGUI.h"

const buttonTexturePath scrollBarTopBtnPath = {"images/scrollTopBtn.png", "images/scrollTopBtnPressed.png"};
const buttonTexturePath scrollBarButtomBtnPath = {"images/scrollBottomBtn.png", "images/scrollBottomBtnPressed.png"};
const buttonTexturePath scrollThumbBtnPath = {"images/scrollThumbBtn.png", "images/scrollThumbBtnPressed.png"};


class ScrollBar : public Window {
    static constexpr double BUTTON_LAYOUT_SHARE_ = 0.1; 

    std::function<void(double)> onScroll_;

    double percentage_ = 0; 
    bool isHorizontal_;

    Button *bottomButton_   = nullptr;
    Button *topButton_      = nullptr;
    Button *thumbButton_    = nullptr;

    void move(double deltaPercent) {

    }

public:
    ScrollBar(int width,  int height, std::function<void(double)> onScroll, bool isHorizontal = true, Widget *parent=nullptr): 
        Window(width, height, parent), onScroll_(onScroll), isHorizontal_(isHorizontal) 
    {


    gm_dot<int, 2> buttonSize
    (
        ( isHorizontal ? BUTTON_LAYOUT_SHARE_ : 1) * rect_.w,
        (!isHorizontal ? BUTTON_LAYOUT_SHARE_ : 1) * rect_.h
    );

    // if (isHorizontal) {
    //     rect_.GetLeftCorner().x += buttonSize.x;
    //     rect_.GetSize().x -= 2 * buttonSize.x;
    // } else {
    //     rect_.GetLeftCorner().y += buttonSize.y;
    //     rect_.GetSize().y -= 2 * buttonSize.y;
    // }

    topButton_ = new Button(buttonSize.x, buttonSize.y, scrollBarTopBtnPath.unpressed, scrollBarTopBtnPath.pressed, nullptr, this);
    addWidget(( isHorizontal ? rect_.w: 0), (!isHorizontal ? -buttonSize.y : 0), topButton_);

    
    bottomButton_ = new Button(buttonSize.x, buttonSize.y, scrollBarButtomBtnPath.unpressed, scrollBarButtomBtnPath.pressed, nullptr, this);
    addWidget((isHorizontal ? -buttonSize.x   : 0), (!isHorizontal ? rect_.h : 0), bottomButton_);

    thumbButton_ = new Button(buttonSize.x, buttonSize.y, scrollThumbBtnPath.unpressed, scrollThumbBtnPath.pressed, nullptr, this);
    addWidget(0, (!isHorizontal ? rect_.h - buttonSize.y: 0), thumbButton_);
    
    }

    // virtual bool onMouseDownSelfAction(const MouseButtonEvent &event) override {

    // }

    // virtual bool onMouseUpSelfAction(const MouseButtonEvent &event) override {

    // }

    // virtual bool onMouseMoveSelfAction(const MouseMotionEvent &event) override {

    // }

};


#endif // SCROL_BAR_H