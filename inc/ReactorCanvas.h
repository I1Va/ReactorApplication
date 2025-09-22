#ifndef REACTORCANVAS_H
#define REACTORCANVAS_H

#include "MyGUI.h"
#include "ReactorModel.h"
#include "SDL2/SDL2_gfxPrimitives.h"

// class MGShape {
//     SDL_Point position_;
//     SDL_Color color_;

// public:
//     MGShape(const SDL_Point &position, const SDL_Color &color): position_(position), color_(color) {};
//     ~MGShape() = default;

//     virtual void draw(SDL_Renderer* renderer) const {
//         assert(renderer);
//     }
// };

// class MGCircle : MGShape {
//     double radius_;

// public:
//     MGCircle(const SDL_Point &position, const double radius, const SDL_Color &color):
//         MGShape(position, color), radius_(radius) {};
// }


class ReactorCanvas : public MGWidget {
    ReactorModel reactor_ = {};

    bool nedsRedraw_ = false;
    
    // gm_vector<Shape *> geomPrimitives;

public:
    void redrawRector() {
        // change current canvas state
    }

    ReactorCanvas
    (
        const int width, const int height,
        std::optional<unsigned int> seed=std::nullopt,
        const MGWindow *parent=nullptr
    ) : 
        MGWidget(width, height, parent),
        reactor_(double(width), double(height), [this]() {signalManager_->emit("reactor_updated");}, seed)
    {
        signalManager_->connect("reactor_updated", [this]() {
            this->nedsRedraw_ = true;
        });
    }
private:
    void paintEvent(SDL_Renderer* renderer) override {
        SDL_Rect widgetRect = {0, 0, width_, height_};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // MGCanvas BACKGROUND COLOR
        SDL_RenderFillRect(renderer, &widgetRect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderDrawLine(renderer, 0, 0, 1000, 1000);
    }
}


#endif // REACTORCANV