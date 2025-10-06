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
    bool needReSize_ = false;
    
    int reactorWidth_;
    int reactorHeight_;
    ReactorModel reactorModel_;
    
    std::vector<MGShape *> geomPrimitives_ = {};
    ReactorWallWidget *leftWall     = nullptr;  
    ReactorWallWidget *rightWall    = nullptr; 
    ReactorWallWidget *topWall      = nullptr;
    ReactorWallWidget *bottomWall   = nullptr;

private:
    void recalculateReactorCanvasSize() {
        leftWall->setSize(REACTOR_WALL_WIDTH, reactorHeight_);
        rightWall->setSize(REACTOR_WALL_WIDTH, reactorHeight_);
        topWall->setSize(reactorWidth_, REACTOR_WALL_WIDTH);
        bottomWall->setSize(reactorWidth_, REACTOR_WALL_WIDTH);

        leftWall->setPosition(0, REACTOR_WALL_WIDTH);
        topWall->setPosition(REACTOR_WALL_WIDTH, 0);
        bottomWall->setPosition(REACTOR_WALL_WIDTH, REACTOR_WALL_WIDTH + reactorHeight_);
        rightWall->setPosition(REACTOR_WALL_WIDTH + reactorWidth_, REACTOR_WALL_WIDTH);

        leftWall->setRerenderFlag();
        topWall->setRerenderFlag();
        bottomWall->setRerenderFlag();
        rightWall->setRerenderFlag();

        needReSize_ = false;
    }
    
    void createReactorWalls() {
        leftWall   = new ReactorWallWidget(0, 0, REACTOR_WALL_COLOR, this);
        rightWall  = new ReactorWallWidget(0, 0, REACTOR_WALL_COLOR, this);
        topWall    = new ReactorWallWidget(0, 0, REACTOR_WALL_COLOR, this);
        bottomWall = new ReactorWallWidget(0, 0, REACTOR_WALL_COLOR, this);

    
        addWidget(0, 0, leftWall);

        addWidget(0, 0, topWall);
        addWidget(0, 0, bottomWall);

        addWidget(0, 0, rightWall);

        recalculateReactorCanvasSize();
    }

public:
    ReactorCanvas
    (
        int width, int height,
        Widget *parent=nullptr
    ) : 
        Container(width, height, parent),
        reactorWidth_(width - 2 * REACTOR_WALL_WIDTH),
        reactorHeight_(height - 2 * REACTOR_WALL_WIDTH),
        reactorModel_(reactorWidth_, reactorHeight_)
    {
        createReactorWalls();  
    }

    ~ReactorCanvas() override {
        for (MGShape *shape : geomPrimitives_) delete shape;
    }

    ReactorModel *reactorModel() { return &reactorModel_; }

    void setRecalcFlag() { needReCalc_ = true; }
    void setUpdateSizeFlag() { needReSize_ = true; }

    void recalculateMoleculePrimitives() {
        for (auto primitive : geomPrimitives_) delete primitive;
        geomPrimitives_.clear();

        for (auto molecule : reactorModel_.getMolecules()) {
            MGShape *curPrimitive = nullptr;
        
            switch (molecule->getType()) {
    
                case MoleculeTypes::CIRCLIT:
                    curPrimitive = (MGShape *) new MGCircle(
                        {(int) molecule->getPosition().get_x() + REACTOR_WALL_WIDTH, (int) molecule->getPosition().get_y() + REACTOR_WALL_WIDTH},
                        molecule->getSize(), CIRCLIT_COLOR);
    
                    geomPrimitives_.push_back(curPrimitive);
                    break;
                case MoleculeTypes::QUADRIT:
                    curPrimitive = (MGShape *) new MGSquare(
                        {(int) molecule->getPosition().get_x() + REACTOR_WALL_WIDTH, (int) molecule->getPosition().get_y() + REACTOR_WALL_WIDTH},
                        molecule->getSize(), QUADRIT_COLOR);
        
                    geomPrimitives_.push_back(curPrimitive);
                    break;
                default:
                    assert(0 && "ReactorCanvas update() : unknown moleculeType");
                    break;
            }
        }
        needReCalc_ = false;
    }

    bool updateSelfAction() override {
        if (!(needReCalc_ || needReSize_)) return false;

        if (needReSize_) recalculateReactorCanvasSize();
        if (needReCalc_) recalculateMoleculePrimitives();
        
        setRerenderFlag();

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
    
        SDL_Rect widgetRect = {0, 0, rect_.w, rect_.h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // MGCanvas BACKGROUND COLOR
        SDL_RenderFillRect(renderer, &widgetRect);

        for (auto shape : geomPrimitives_) {
            shape->draw(renderer);
        }
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
        reactorWidth_ = std::max(MIN_REACTOR_SIZE, reactorWidth_ - NARROWING_DELTA);
        setUpdateSizeFlag();
    }
    void unNarrowRightWall() {
        reactorModel_.narrowRightWall(-NARROWING_DELTA);
        reactorWidth_ = std::max(MIN_REACTOR_SIZE, reactorWidth_ + NARROWING_DELTA);
        
        setUpdateSizeFlag();
    }
};

class ReactorVisibleArea : public Container {
    ReactorCanvas *ReactorCanvas_ = nullptr;
public:
    ReactorVisibleArea (int visibleWidth, int visibleHeight, Widget *parent=nullptr): 
        Container(visibleWidth, visibleHeight, parent) {}

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

    int reactorUpdateDelayMS_;

    ReactorModel *reactorModel_ = nullptr;
    ReactorCanvas *reactorCanvas_ = nullptr;

private:
    Container *createReactorButtonPanel(int width, int height) {
        int buttonsCount = 10;
    
        int buttonWidth = width / (buttonsCount / 2);
        int buttonHeight = height / 2;


        Container *buttonPanel = new Container(width, height, this);
        
       
        Button *addCirclitBtn        = new Button(buttonWidth, buttonHeight, ADD_CIRCLIT, ADD_CIRCLIT_PRESSED, [this](){ reactorCanvas_->addCirclit(); }, buttonPanel);
        Button *addQuadritBtn        = new Button(buttonWidth, buttonHeight, ADD_QUADRIT, ADD_QUADRIT_PRESSED, [this](){ reactorCanvas_->addQuadrit(); }, buttonPanel);
        Button *removeMoleculeBtn    = new Button(buttonWidth, buttonHeight, REMOVE_MOLECULE, REMOVE_MOLECULE_PRESSED, [this](){ reactorCanvas_->removeMolecule(); }, buttonPanel);
        Button *narrowRightWallBtn   = new Button(buttonWidth, buttonHeight, NARROW_RIGHTWALL, NARROW_RIGHTWALL_PRESSED, [this](){ reactorCanvas_->narrowRightWall(); }, buttonPanel);
        Button *unNarrowRightWallBtn = new Button(buttonWidth, buttonHeight, UNNARROW_RIGHTWALL, UNNARROW_RIGHTWALL_PRESSED, [this](){ reactorCanvas_->unNarrowRightWall(); }, buttonPanel);

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
        reactorUpdateDelayMS_(reactorUpdateDelayMS)
    {
        
        ReactorVisibleArea *reactorVisibleArea = new ReactorVisibleArea(REACTOR_CANVAS_WIDTH, REACTOR_CANVAS_HEIGHT, this);
        reactorCanvas_ = new ReactorCanvas(REACTOR_CANVAS_WIDTH, REACTOR_CANVAS_HEIGHT, reactorVisibleArea);
        
        
        reactorVisibleArea->addWidget(0, 0, reactorCanvas_);
        reactorModel_ = reactorCanvas_->reactorModel();

    
        int buttonPanelWidth   = REACTOR_GUI_WIDTH - 2 * BORDER_SIZE;
        int buttonPanelHeight  = REACTOR_GUI_HEIGHT - REACTOR_CANVAS_HEIGHT - 3 * BORDER_SIZE;
        Container *ButtonPanel = createReactorButtonPanel(buttonPanelWidth, buttonPanelHeight);
    
        addWidget(BORDER_SIZE, BORDER_SIZE, reactorVisibleArea);
        addWidget(BORDER_SIZE, BORDER_SIZE * 2 + REACTOR_CANVAS_HEIGHT, ButtonPanel);
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_SetRenderDrawColor(renderer, REACTOR_COLOR.r, REACTOR_COLOR.g, REACTOR_COLOR.b, REACTOR_COLOR.a); 
        SDL_Rect full = {0, 0, rect_.w, rect_.h};
        SDL_RenderFillRect(renderer, &full);
    }

     void updateReactor(int deltaMS) {
        static int passedDeltaMS = 0;

        passedDeltaMS += deltaMS;

        if (passedDeltaMS >= reactorUpdateDelayMS_) {
            reactorModel_->update(double(reactorUpdateDelayMS_) / SEC_TO_MS);
            reactorCanvas_->setRecalcFlag();
            passedDeltaMS -= reactorUpdateDelayMS_;
        }
    }
    int reactorUpdateDelayMS() const { return reactorUpdateDelayMS_; }

};


#endif // REACTOR_GUI_H