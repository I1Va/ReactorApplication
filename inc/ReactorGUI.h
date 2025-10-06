#ifndef REACTOR_GUI_H
#define REACTOR_GUI_H

#include "MyGUI.h"
#include "ReactorModel.h"
#include "SDL2/SDL2_gfxPrimitives.h"

const SDL_Color CIRCLIT_COLOR = {255, 0, 0, 255};
const SDL_Color QUADRIT_COLOR = {0, 0, 255, 255};
const SDL_Color REACTOR_COLOR = {220, 220, 220, 255};
const SDL_Color REACTOR_WALL_COLOR = {150, 150, 150, 255};
const int REACTOR_WALL_WIDTH = 10;

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

class ReactorWallWidget : public Widget {
    double currentEnergy_ = 1;
    double systemSummaryEnergy_ = 1;
    SDL_Color wallStartColor_ = {};
    Uint8 redColorPart = 0;

public:
    ReactorWallWidget(int width, int height, SDL_Color wallColor, Widget *parent=nullptr): Widget(width, height, parent), wallStartColor_(wallColor) {}

    void setWallEnergyPair(double currentEnergy, double systemSummaryEnergy) {
        currentEnergy_ = currentEnergy;
        systemSummaryEnergy_ = systemSummaryEnergy;
        redColorPart = std::max((Uint8) 255,  (Uint8) (wallStartColor_.r + (255 * currentEnergy_ / systemSummaryEnergy_)));
        setRerenderFlag();
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_Rect widgetRect = {0, 0, rect_.w, rect_.h};
        SDL_SetRenderDrawColor(renderer, redColorPart, wallStartColor_.g, wallStartColor_.b, wallStartColor_.a);
        SDL_RenderFillRect(renderer, &widgetRect);
    }
};

class ReactorCanvas : public Container {
    bool needReCalc_ = false;
    const ReactorModel& reactorModel_;
    

    // visible area of ​​the rector and walls
    // wall1, wall2, wall3, wall4 
    std::vector<MGShape *> geomPrimitives_ = {}; // molecules

public:
    ReactorCanvas
    (
        const ReactorModel& reactorModel,
        Widget *parent=nullptr
    ) : 
        Container(reactorModel.getWidth(), reactorModel.getHeight(), parent),
        reactorModel_(reactorModel)
    {
        ReactorWallWidget *leftWall   = new ReactorWallWidget(REACTOR_WALL_WIDTH, rect_.h, REACTOR_WALL_COLOR, this);
        ReactorWallWidget *rightWall  = new ReactorWallWidget(REACTOR_WALL_WIDTH, rect_.h, REACTOR_WALL_COLOR, this);
        ReactorWallWidget *topWall    = new ReactorWallWidget(rect_.w, REACTOR_WALL_WIDTH, REACTOR_WALL_COLOR, this);
        ReactorWallWidget *bottomWall = new ReactorWallWidget(rect_.w, REACTOR_WALL_WIDTH, REACTOR_WALL_COLOR, this);

    
        addWidget(0, REACTOR_WALL_WIDTH, leftWall);
    
        addWidget(REACTOR_WALL_WIDTH, 0, topWall);
        addWidget(REACTOR_WALL_WIDTH, REACTOR_WALL_WIDTH + rect_.h, bottomWall);

        addWidget(REACTOR_WALL_WIDTH + rect_.w, REACTOR_WALL_WIDTH, rightWall);
    }

    ~ReactorCanvas() override {
        for (MGShape *shape : geomPrimitives_) delete shape;
    }

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
                    assert(0 && "ReactorCanvas update() : unknown moleculeType");
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

class ReactorVisibleArea : public Container {
    ReactorCanvas *ReactorCanvas_ = nullptr;
public:
    ReactorVisibleArea
    (
        int canvasWidth, int CanvasHeight, 
        const ReactorModel& reactorModel,
        Widget *parent=nullptr
    ): 
        Container(canvasWidth, CanvasHeight, parent) 
    {
        ReactorCanvas_ = new ReactorCanvas(reactorModel, this);
        
        addWidget(0, 0, ReactorCanvas_);
    }

    void setRecalcState() {
        ReactorCanvas_->setRecalcState();
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_SetRenderDrawColor(renderer, REACTOR_COLOR.r, REACTOR_COLOR.g, REACTOR_COLOR.b, REACTOR_COLOR.a); 
        SDL_Rect full = {0, 0, rect_.w, rect_.h};
        SDL_RenderFillRect(renderer, &full);
    }

};

class ReactorGUI : public Window {
    static constexpr const int BORDER_SIZE = 20;
    static constexpr const int REACTOR_GUI_HEIGHT = 450;
    static constexpr const int REACTOR_GUI_WIDTH = 350;

    static constexpr const int REACTOR_CANVAS_HEIGHT = 300;
    static constexpr const int REACTOR_CANVAS_WIDTH = REACTOR_GUI_WIDTH - 2 * BORDER_SIZE;

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

    ReactorVisibleArea *ReactorVisibleArea_ = nullptr;

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
    
        ReactorVisibleArea_ = new ReactorVisibleArea(REACTOR_CANVAS_WIDTH, REACTOR_CANVAS_HEIGHT, 
                                                     reactorModel_, this);
        
        addWidget(BORDER_SIZE, BORDER_SIZE, ReactorVisibleArea_);

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
            ReactorVisibleArea_->setRecalcState();
            passedDeltaMS -= reactorUpdateDelayMS_;
        }
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_SetRenderDrawColor(renderer, REACTOR_COLOR.r, REACTOR_COLOR.g, REACTOR_COLOR.b, REACTOR_COLOR.a); 
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


#endif // REACTOR_GUI_H