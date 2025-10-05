#ifndef REACTORCANVAS_H
#define REACTORCANVAS_H

#include "MyGUI.h"
#include "ReactorModel.h"
#include "SDL2/SDL2_gfxPrimitives.h"

const SDL_Color CIRCLIT_COLOR = {255, 0, 0, 255};
const SDL_Color QUADRIT_COLOR = {0, 0, 255, 255};
const double NARROWING_DELTA = 10;
const int SEC_TO_MS = 1000;

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

        SDL_Rect square = {position_.x - length_ / 2, position_.y - length_ / 2, length_, length_};
        SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);
        SDL_RenderFillRect(renderer, &square);
    }
};

class ReactorCanvas : public Widget {
    ReactorModel reactor_ = {};

    bool nedsRedraw_ = false;
    int reactorUpdateMS_;
    
    std::vector<MGShape *> geomPrimitives_ = {};

public:
    ReactorCanvas
    (
        const int width, const int height,
        std::optional<unsigned int> seed=std::nullopt,
        int reactorUpdateMS=40, Widget *parent=nullptr
    ) : 
        Widget(width, height, parent),
        reactor_(double(width), double(height), nullptr, seed),
        reactorUpdateMS_(reactorUpdateMS)
    {}

    bool updateSelfAction() override {
        if (!nedsRedraw_) return false;
        nedsRedraw_ = false;

        for (auto primitive : geomPrimitives_) delete primitive;
        geomPrimitives_.clear();

        for (auto molecule : reactor_.getMolecules()) {
            MGShape *curPrimitive = nullptr;
        
            switch (molecule->getType()) {
                case MoleculeTypes::CIRCLIT:
                    curPrimitive = (MGShape *) new MGCircle(
                        {(int) molecule->getPosition().get_x(), (int) molecule->getPosition().get_y()},
                        molecule->getSize(), CIRCLIT_COLOR);
    
                    geomPrimitives_.push_back(curPrimitive);
                    break;
                case MoleculeTypes::QUADRIT:
                    curPrimitive = (MGShape *) new MGSquare(
                        {(int) molecule->getPosition().get_x(), (int) molecule->getPosition().get_y()}, 
                        molecule->getSize(), QUADRIT_COLOR);
        
                    geomPrimitives_.push_back(curPrimitive);
                    break;
                default:
                    assert(0 && "ReactorCanvas update() : unknown moleculeType");
                    break;
            }
        }
        return true;
    }

    void addCirclit() {
        reactor_.addCirclit();
    }
    void addQuadrit() {
        reactor_.addQuadrit();
    }
    void removeMolecule() {
        reactor_.removeMolecule();
    }
    void narrowRightWall() {
        reactor_.narrowRightWall(NARROWING_DELTA);
    }
    void unNarrowRightWall() {
        reactor_.narrowRightWall(-NARROWING_DELTA);
    }

    void showInfo() {
        double summaryEnergy = 0;
        for (auto molecule : reactor_.getMolecules()) {
            summaryEnergy += molecule->getKinecticEnergy() + molecule->getPotentialEnergy();
        }
        for (size_t i = 0; i < 4; i++) {
            std::cout << "wall[" << i << "].energy = " << reactor_.getReactorWalls()[i].energy << ", ";
        }

        std::cout << "systemEnergy : " << summaryEnergy << "\n";
        for (size_t i = 0; i < 4; i++) {
            std::cout << "wall[" << i << "].energy = " << reactor_.getReactorWalls()[i].energy << ", ";
        }
        std::cout << "\n\n";
    }
    
    void reactorUpdate(int deltaMS) {
        static int passedDeltaMS = 0;

        passedDeltaMS += deltaMS;

        if (passedDeltaMS >= reactorUpdateMS_) {
            reactor_.update(reactorUpdateMS_ / SEC_TO_MS);
            passedDeltaMS -= reactorUpdateMS_;
            rect_.w = (int) reactor_.getWidth();
            rect_.h = (int) reactor_.getHeight();
            showInfo();
        }
    }

private:
    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_Rect widgetRect = {0, 0, rect_.w, rect_.h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // MGCanvas BACKGROUND COLOR
        SDL_RenderFillRect(renderer, &widgetRect);

        for (auto shape : geomPrimitives_) {
            shape->draw(renderer);
        }
    }
};


#endif // REACTORCANVAS_H