#ifndef ReactorView_H
#define ReactorView_H

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
protected:
    SDL_Point position_ = {};
    SDL_Color color_ = {};

public:
    MGShape(const SDL_Point &position, const SDL_Color &color): position_(position), color_(color) {};
    virtual ~MGShape() = default;

    virtual void draw(SDL_Renderer* renderer) const = 0;
    SDL_Point position() const { return position_; }
};

class MGCircle : public MGShape {
    int radius_;

public:
    MGCircle(const SDL_Point &position, const int radius, const SDL_Color &color):
        MGShape(position, color), radius_(radius) {};

    void draw(SDL_Renderer* renderer) const override {
        RendererGuard rendererGuard(renderer);
        
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

class ReactorView : public Container {
    bool needReCalc_ = false;
    const ReactorModel& reactorModel_;
    

    // visible area of ​​the rector and walls
    // wall1, wall2, wall3, wall4 
    std::vector<MGShape *> geomPrimitives_ = {}; // molecules

public:
    ReactorView
    (
        const int width, const int height,
        const ReactorModel& reactorModel,
        Widget *parent=nullptr
    ) : 
        Container(width, height, parent),
        reactorModel_(reactorModel)
    {}

    void setRecalcState() { needReCalc_ = true; }

    bool updateSelfAction() override {
        if (!needReCalc_) return false;
        needReCalc_ = false;
        setRerenderFlag();
        
        setSize(reactorModel_.getWidth(), reactorModel_.getHeight());

        for (auto primitive : geomPrimitives_) delete primitive;
        geomPrimitives_.clear();

        for (auto molecule : reactorModel_.getMolecules()) {
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
                    assert(0 && "ReactorView update() : unknown moleculeType");
                    break;
            }
        }
        return true;
    }

    void showInfo() {
        double summaryEnergy = 0;
        for (auto molecule : reactorModel_.getMolecules()) {
            summaryEnergy += molecule->getKinecticEnergy() + molecule->getPotentialEnergy();
        }
    
        for (size_t i = 0; i < 4; i++) {
            std::cout << "wall[" << i << "].energy = " << reactorModel_.getReactorWalls()[i].energy << ", ";
        }

        std::cout << "\nsystemEnergy : " << summaryEnergy << "\n";
        for (size_t i = 0; i < 4; i++) {
            std::cout << "wall[" << i << "].energy = " << reactorModel_.getReactorWalls()[i].energy << ", ";
        }

        // for (auto molecule : reactorModel_.getMolecules()) {
        //     std::cout << molecule->getPosition() << " ";
        // }
        // std::cout << "\n";
    
        std::cout << "\n\n";
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        // showInfo();

        SDL_Rect widgetRect = {0, 0, rect_.w, rect_.h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // MGCanvas BACKGROUND COLOR
        SDL_RenderFillRect(renderer, &widgetRect);

        for (auto shape : geomPrimitives_) {
            shape->draw(renderer);
        }
    }
};

class ReactorCanvas : public Container {
    ReactorView *reactorView_ = nullptr;
public:
    ReactorCanvas
    (
        int canvasWidth, int CanvasHeight, 
        int ReactorWidth, int ReactorHeight, const ReactorModel& reactorModel,
        Widget *parent=nullptr
    ): 
        Container(canvasWidth, CanvasHeight, parent) 
    {
        reactorView_ = new ReactorView(ReactorWidth, ReactorHeight, reactorModel, this);
        addWidget(0, 0, reactorView_);
    }

    void setRecalcState() {
        reactorView_->setRecalcState();
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
        SDL_Rect full = {0, 0, rect_.w, rect_.h};
        SDL_RenderFillRect(renderer, &full);
    }

};

class ReactorGUI : public Window {
    static constexpr const int BORDER_SIZE = 10;
    static constexpr const int REACTOR_GUI_HEIGHT = 400;
    static constexpr const int REACTOR_GUI_WIDTH = 350;

    static constexpr const int REACTOR_CANVAS_HEIGHT = 300;
    static constexpr const int REACTOR_CANVAS_WIDTH = REACTOR_GUI_WIDTH - 3 * BORDER_SIZE;

    static constexpr const char NARROW_RIGHTWALL[] = "images/NarrowRightWall.png";
    static constexpr const char NARROW_RIGHTWALL_PRESSED[] = "images/NarrowRightWallPressed.png";
    static constexpr const char UNNARROW_RIGHTWALL[] = "images/UnNarrowRightWall.png";
    static constexpr const char UNNARROW_RIGHTWALL_PRESSED[] = "images/UnNarrowRightWalPressed.png";

    static constexpr const char ADD_CIRCLIT_PRESSED[] = "images/addCirclitPressed.png";
    static constexpr const char ADD_CIRCLIT[] = "images/addCirclitUnPressed.png";
    static constexpr const char ADD_QUADRIT_PRESSED[] = "images/addQuadritPressed.png";
    static constexpr const char ADD_QUADRIT[] = "images/addQuadritUnPressed.png";
    static constexpr const char REMOVE_MOLECULE[] = "images/removeMolecule.png";
    static constexpr const char REMOVE_MOLECULE_PRESSED[] = "images/removeMoleculePressed.png";


    ReactorModel reactorModel_;
    int reactorUpdateDelayMS_;

    ReactorCanvas *reactorCanvas_ = nullptr;

private:
    Container *createReactorButtonPanel(int width, int height) {
        int buttonsCount = 10;
    
        int buttonWidth = width / (buttonsCount / 2);
        int buttonHeight = height / 2;


        Container *buttonPanel = new Container(width, height, this);
        
       
        Button *addCirclitBtn        = new Button(buttonWidth, buttonHeight, ADD_CIRCLIT, ADD_CIRCLIT_PRESSED, [this](){ addCirclit(); }, buttonPanel);
        Button *addQuadritBtn        = new Button(buttonWidth, buttonHeight, ADD_QUADRIT, ADD_QUADRIT_PRESSED, [this](){ addQuadrit(); }, buttonPanel);
        Button *removeMoleculeBtn    = new Button(buttonWidth, buttonHeight, REMOVE_MOLECULE, REMOVE_MOLECULE_PRESSED, [this](){ removeMolecule(); }, buttonPanel);
        Button *narrowRightWallBtn   = new Button(buttonWidth, buttonHeight, NARROW_RIGHTWALL, NARROW_RIGHTWALL_PRESSED, [this](){ narrowRightWall(); }, buttonPanel);
        Button *unNarrowRightWallBtn = new Button(buttonWidth, buttonHeight, UNNARROW_RIGHTWALL, UNNARROW_RIGHTWALL_PRESSED, [this](){ unNarrowRightWall(); }, buttonPanel);

        std::vector<Button *> buttons = 
        {
            addCirclitBtn,
            addQuadritBtn,
            removeMoleculeBtn,
            narrowRightWallBtn,
            unNarrowRightWallBtn
        };

        int curBtnX = 0;
        int curBtnY = 0;
    
        for (Button *button : buttons) {
            buttonPanel->addWidget(curBtnX, curBtnY, button);
            curBtnX += buttonWidth;
            if (curBtnX > width) {
                curBtnX = 0;
                curBtnY += buttonHeight;
            }
        }

        return buttonPanel;
    }

public:
    ReactorGUI(int reactorUpdateDelayMS=40): 
        Window(REACTOR_GUI_WIDTH, REACTOR_GUI_HEIGHT),
        reactorUpdateDelayMS_(reactorUpdateDelayMS),
        reactorModel_(REACTOR_CANVAS_WIDTH, REACTOR_CANVAS_HEIGHT, nullptr, std::nullopt) 
    {
       
        int REACTOR_MODEL_WIDTH = REACTOR_CANVAS_WIDTH;
        int REACTOR_MODEL_HEIGHT = REACTOR_CANVAS_HEIGHT;
        reactorCanvas_ = new ReactorCanvas(REACTOR_CANVAS_WIDTH, REACTOR_CANVAS_HEIGHT, 
                                           REACTOR_MODEL_WIDTH, REACTOR_MODEL_HEIGHT,
                                           reactorModel_, this);
        
        addWidget(BORDER_SIZE, BORDER_SIZE, reactorCanvas_);

        int buttonPanelWidth    =  REACTOR_GUI_WIDTH - 2 * BORDER_SIZE;
        int buttonPanelHeight   = REACTOR_GUI_HEIGHT - REACTOR_CANVAS_HEIGHT - 3 * BORDER_SIZE;

        Container *ButtonPanel = createReactorButtonPanel(buttonPanelWidth, buttonPanelHeight);

        addWidget(BORDER_SIZE, BORDER_SIZE * 2 + REACTOR_CANVAS_HEIGHT, ButtonPanel);
    }

    void updateReactor(int deltaMS) {
        static int passedDeltaMS = 0;

        passedDeltaMS += deltaMS;

        if (passedDeltaMS >= reactorUpdateDelayMS_) {
            reactorModel_.update(double(reactorUpdateDelayMS_) / SEC_TO_MS);
            reactorCanvas_->setRecalcState();
            passedDeltaMS -= reactorUpdateDelayMS_;
        }
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
        SDL_Rect full = {0, 0, rect_.w, rect_.h};
        SDL_RenderFillRect(renderer, &full);
    }

    void addCirclit() {
        reactorModel_.addCirclit();
    }
    void addQuadrit() {
        reactorModel_.addQuadrit();
    }
    void removeMolecule() {
        reactorModel_.removeMolecule();
    }
    void narrowRightWall() {
        reactorModel_.narrowRightWall(NARROWING_DELTA);
    }
    void unNarrowRightWall() {
        reactorModel_.narrowRightWall(-NARROWING_DELTA);
    }
    int reactorUpdateDelayMS() const { return reactorUpdateDelayMS_; }

};


#endif // ReactorView_H