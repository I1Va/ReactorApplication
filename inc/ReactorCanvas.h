#ifndef REACTORCANVAS_H
#define REACTORCANVAS_H

#include "MyGUI.h"
#include "ReactorModel.h"
#include "SDL2/SDL2_gfxPrimitives.h"

Uint32 SDLColorToUint32(SDL_Color c) {
    return ((Uint32)c.r << 24) | 
           ((Uint32)c.g << 16) | 
           ((Uint32)c.b << 8)  | 
           ((Uint32)c.a);
}

class MGShape {
public:
    MGShape(const SDL_Point &position, const SDL_Color &color): position_(position), color_(color) {};
    virtual ~MGShape() = default;

    virtual void draw(SDL_Renderer* renderer) const = 0;

protected:
    SDL_Point position_ = {};
    SDL_Color color_ = {};
};

class MGCircle : public MGShape {
    int radius_;

public:
    MGCircle(const SDL_Point &position, const int radius, const SDL_Color &color):
        MGShape(position, color), radius_(radius) {};

    void draw(SDL_Renderer* renderer) const override {
        filledCircleColor(renderer, (Sint16) position_.x, (Sint16) position_.y, (Sint16) radius_, SDLColorToUint32(color_));
    }
};

class MGSquare : public MGShape {
    int length_;

public:
    MGSquare(const SDL_Point &position, const int length, const SDL_Color &color):
        MGShape(position, color), length_(length) {};

    void draw(SDL_Renderer* renderer) const override {
        RendererGuard rendererGuard(renderer);

        SDL_Rect square = {position_.x - length_ / 2, position_.y - length_ / 2, position_.x + length_ / 2, position_.y + length_ / 2 };
        SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);
        SDL_RenderFillRect(renderer, &square);
    }
};

class ReactorCanvas : public MGWidget {
    ReactorModel reactor_ = {};

    bool nedsRedraw_ = false;
    
    std::vector<MGShape *> geomPrimitives;

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
        reactor_(double(width), double(height), nullptr, seed)
    {}

    void setSignalManager(SignalManager *const signalManager) override {
        signalManager_ = signalManager;

        signalManager_->connect("reactor_updated", [this]() {
            this->nedsRedraw_ = true;
        });

        reactor_.setOnUpdate([this]() {signalManager_->emit("reactor_updated");});
    }

private:
    void paintEvent(SDL_Renderer* renderer) override {
        SDL_Rect widgetRect = {0, 0, width_, height_};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // MGCanvas BACKGROUND COLOR
        SDL_RenderFillRect(renderer, &widgetRect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderDrawLine(renderer, 0, 0, 1000, 1000);
    }
};


#endif // REACTORCANVAS_H