#ifndef REACTORCANVAS_H
#define REACTORCANVAS_H

#include "MyGUI.h"
#include "ReactorModel.h"


class ReactorCanvas : public MGWidget {
    ReactorModel reactor = {}; 
public:
// ReactorModel
//     (       
//         const double width, const double height,
//         std::function<void(ReactorModel&)> onUpdate=nullptr,
//         std::optional<unsigned int> seed=std::nullopt
//     ):
//         width_(width), height_(height),
//         onUpdate_(onUpdate),
//         randomGenerator_(seed.value_or(std::random_device{}()))
//     {}

    ReactorCanvas(c)
private:
    void paintEvent(SDL_Renderer* renderer) override {
        SDL_Rect widgetRect = {0, 0, width_, height_};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // MGCanvas BACKGROUND COLOR
        SDL_RenderFillRect(renderer, &widgetRect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderDrawLine(renderer, 0, 0, 1000, 1000);
    }
}






#endif // REACTORCANVAS_H