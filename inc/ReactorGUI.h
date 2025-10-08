#ifndef REACTOR_GUI_H
#define REACTOR_GUI_H

#include "MyGUI.h"
#include "ReactorModel.h"
#include "SDL2/SDL2_gfxPrimitives.h"

const SDL_Color CIRCLIT_COLOR = {255, 0, 0, 255};
const SDL_Color QUADRIT_COLOR = {0, 0, 255, 255};
const int REACTOR_WALL_WIDTH = 10;
const double REACTOR_WALL_TEMPERATURE_COLOR_COEF = 1.0 / 400000;
const double NARROWING_DELTA = 10;
const int SEC_TO_MS = 1000;
const int EXPLODE_PARTICLES_NUM = 100;

struct ReactorButtonTexturePack {
    
    ButtonTexturePath narrowRightWallBtnPath;
    ButtonTexturePath unNarrowRightWallBtnPath;
    ButtonTexturePath addCirclitBtnPath;
    ButtonTexturePath addQuadritBtnPath;
    ButtonTexturePath removeMoleculeBtnPath;

    ButtonTexturePath heatTopWallBtnPath; 
    ButtonTexturePath heatBottomWallBtnPath;  
    ButtonTexturePath heatLeftWallBtnPath;
    ButtonTexturePath heatRightWallBtnPath;

    ButtonTexturePath explodeReactorBtnPath;
};

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
        filledCircleColor(renderer, (Sint16) position_.x, (Sint16) position_.y, (Sint16) radius_, SDL2gfxColorToUint32(color_));
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
    Uint8 redColorPart = 0;

public:
    ReactorWallWidget(int width, int height, Widget *parent=nullptr): Widget(width, height, parent) {}

    void setWallEnergyPair(double currentEnergy, double systemSummaryEnergy) {
        currentEnergy_ = currentEnergy;
        systemSummaryEnergy_ = systemSummaryEnergy;
        redColorPart = std::clamp((int) (255 * currentEnergy_ * REACTOR_WALL_TEMPERATURE_COLOR_COEF), 0, 255);
        setRerenderFlag();
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_Rect widgetRect = {0, 0, rect_.w, rect_.h};
        SDL_SetRenderDrawColor(renderer, redColorPart, 0, 0, 255);
        SDL_RenderFillRect(renderer, &widgetRect);
    }
};

class ReactorCanvas : public Container {
    bool needReCalc_ = false;
    bool needReSize_ = false;
    bool needExplode_ = false;
    
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
        leftWall   = new ReactorWallWidget(0, 0, this);
        rightWall  = new ReactorWallWidget(0, 0, this);
        topWall    = new ReactorWallWidget(0, 0, this);
        bottomWall = new ReactorWallWidget(0, 0, this);

    
        addWidget(0, 0, leftWall);

        addWidget(0, 0, topWall);
        addWidget(0, 0, bottomWall);

        addWidget(0, 0, rightWall);

        recalculateReactorCanvasSize();
    }

    void explodeReactor(SDL_Renderer* renderer) { 
        for (int i = 0; i < EXPLODE_PARTICLES_NUM; i++) {
            addCirclit();
        }
        needExplode_ = false;
    }

public:
    ReactorCanvas
    (
        int width, int height,
        std::function<void()> onReactorUpdate,
        Widget *parent=nullptr
    ) : 
        Container(width, height, parent),
        reactorWidth_(width - 2 * REACTOR_WALL_WIDTH),
        reactorHeight_(height - 2 * REACTOR_WALL_WIDTH),
        reactorModel_(reactorWidth_, reactorHeight_, onReactorUpdate)
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

    void recalculateWallsEnergy() {
        leftWall->setWallEnergyPair(reactorModel_.getReactorWalls()[LEFT_WALL].energy, reactorModel_.getSummaryEnergy());
        rightWall->setWallEnergyPair(reactorModel_.getReactorWalls()[RIGHT_WALL].energy, reactorModel_.getSummaryEnergy());
        bottomWall->setWallEnergyPair(reactorModel_.getReactorWalls()[BOTTOM_WALL].energy, reactorModel_.getSummaryEnergy());
        topWall->setWallEnergyPair(reactorModel_.getReactorWalls()[TOP_WALL].energy, reactorModel_.getSummaryEnergy());
    }

    bool updateSelfAction() override {
        if (!(needReCalc_ || needReSize_)) return false;

        if (needReSize_) recalculateReactorCanvasSize();
        if (needReCalc_) {
            recalculateMoleculePrimitives();
            recalculateWallsEnergy();
        }
        
        setRerenderFlag();

        return true;
    }

    void showInfo() {
        std::cout << "SummaryEnergy : " << reactorModel_.getSummaryEnergy() << "\n";
        
        for (size_t i = 0; i < 4; i++) {
            std::cout << "wall[" << i << "].energy = " << reactorModel_.getReactorWalls()[i].energy << ", ";
        }
        std::cout << "\n\n";

        // std::cout << "\nsystemEnergy : " << summaryEnergy << "\n";
        // for (size_t i = 0; i < 4; i++) {
        //     std::cout << "wall[" << i << "].energy = " << reactorModel_.getReactorWalls()[i].energy << ", ";
        // }

        // for (auto molecule : reactorModel_.getMolecules()) {
        //     std::cout << molecule->getPosition() << " ";
        // }
        // std::cout << "\n";
    
        // std::cout << "\n\n";
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);
        
        // showInfo();
        if (needExplode_) { explodeReactor(renderer); }
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

    void heatTopWall() { reactorModel_.addEnergyToWall(TOP_WALL, /*percantage*/ 5); }
    void heatRightWall() { reactorModel_.addEnergyToWall(RIGHT_WALL, /*percantage*/ 5); }
    void heatLeftWall() { reactorModel_.addEnergyToWall(LEFT_WALL, /*percantage*/ 5); }
    void heatBottomWall() { reactorModel_.addEnergyToWall(BOTTOM_WALL, /*percantage*/ 5); }
    
    void setExplodeReactorFlag() { needExplode_ = true; }
};

class ReactorVisibleArea : public Container {
    ReactorCanvas *ReactorCanvas_ = nullptr;
public:
    ReactorVisibleArea (int visibleWidth, int visibleHeight, Widget *parent=nullptr): 
        Container(visibleWidth, visibleHeight, parent) {}

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_SetRenderDrawColor(renderer, DEFAULT_WINDOW_COLOR.r, DEFAULT_WINDOW_COLOR.g, DEFAULT_WINDOW_COLOR.b, DEFAULT_WINDOW_COLOR.a); 
        SDL_Rect full = {0, 0, rect_.w, rect_.h};
        SDL_RenderFillRect(renderer, &full);
    }

};

class ReactorGUI : public Window {
    static constexpr const double REACTOR_CANVAS_SHARE = 0.75;


    int reactorCanvasWidth_ = 0;
    int reactorCanvasHeight_ = 0;
    int buttonPanelWidth_ = 0;
    int buttonPanelHeight_ = 0;

    int reactorUpdateDelayMS_;

    ReactorButtonTexturePack texturePack_ = {};

    ReactorModel *reactorModel_ = nullptr;
    ReactorCanvas *reactorCanvas_ = nullptr;

private:
    Container *createReactorButtonPanel(int width, int height) {
        int buttonsCount = 10;
    
        int buttonWidth = width / (buttonsCount / 2);
        int buttonHeight = height / 2;


        Container *buttonPanel = new Container(width, height, this);
        
       
        Button *addCirclitBtn        = new Button(buttonWidth, buttonHeight, 
                                                  texturePack_.addCirclitBtnPath.unpressed , texturePack_.addCirclitBtnPath.pressed, 
                                                  [this](){ reactorCanvas_->addCirclit(); }, buttonPanel);
        
        Button *addQuadritBtn        = new Button(buttonWidth, buttonHeight, 
                                                  texturePack_.addQuadritBtnPath.unpressed , texturePack_.addQuadritBtnPath.pressed,
                                                  [this](){ reactorCanvas_->addQuadrit(); }, buttonPanel);
        
        Button *removeMoleculeBtn    = new Button(buttonWidth, buttonHeight, 
                                                  texturePack_.removeMoleculeBtnPath.unpressed , texturePack_.removeMoleculeBtnPath.pressed,
                                                  [this](){ reactorCanvas_->removeMolecule(); }, buttonPanel);
                                                
        Button *narrowRightWallBtn   = new Button(buttonWidth, buttonHeight, 
                                                  texturePack_.narrowRightWallBtnPath.unpressed, texturePack_.narrowRightWallBtnPath.pressed,
                                                  [this](){ reactorCanvas_->narrowRightWall(); }, buttonPanel);
                                                
        Button *unNarrowRightWallBtn = new Button(buttonWidth, buttonHeight, 
                                                  texturePack_.unNarrowRightWallBtnPath.unpressed, texturePack_.unNarrowRightWallBtnPath.pressed, 
                                                  [this](){ reactorCanvas_->unNarrowRightWall(); }, buttonPanel);
        
        Button *heatTopWallBtn          = new Button(buttonWidth, buttonHeight, 
                                                  texturePack_.heatTopWallBtnPath.unpressed , texturePack_.heatTopWallBtnPath.pressed, 
                                                  [this](){ reactorCanvas_->heatTopWall(); }, buttonPanel);
        
        Button *heatBottomWallBtn       = new Button(buttonWidth, buttonHeight, 
                                                  texturePack_.heatBottomWallBtnPath.unpressed , texturePack_.heatBottomWallBtnPath.pressed,
                                                  [this](){ reactorCanvas_->heatBottomWall(); }, buttonPanel);
        
        Button *heatLeftWallBtn         = new Button(buttonWidth, buttonHeight, 
                                                  texturePack_.heatLeftWallBtnPath.unpressed , texturePack_.heatLeftWallBtnPath.pressed,
                                                  [this](){ reactorCanvas_->heatLeftWall(); }, buttonPanel);
                                                
        Button *heatRighttWallBtn       = new Button(buttonWidth, buttonHeight, 
                                                  texturePack_.heatRightWallBtnPath.unpressed, texturePack_.heatRightWallBtnPath.pressed,
                                                  [this](){ reactorCanvas_->heatRightWall(); }, buttonPanel);
                                                
        Button *explodeReactorBtn       = new Button(buttonWidth, buttonHeight, 
                                                  texturePack_.explodeReactorBtnPath.unpressed, texturePack_.explodeReactorBtnPath.pressed, 
                                                  [this](){ reactorCanvas_->setExplodeReactorFlag(); }, buttonPanel);

        std::vector<Button *> buttons = 
        {
            addCirclitBtn,
            addQuadritBtn,
            removeMoleculeBtn,
            narrowRightWallBtn,
            unNarrowRightWallBtn,

            heatTopWallBtn,
            heatBottomWallBtn,    
            heatLeftWallBtn,
            heatRighttWallBtn,
            explodeReactorBtn
        };

        int curBtnX = 0;
        int curBtnY = 0;
        
        for (Button *button : buttons) {
            if (curBtnX >= buttonPanelWidth_) {
                curBtnX = 0;
                curBtnY += buttonHeight;
            }
            buttonPanel->addWidget(curBtnX, curBtnY, button);
            curBtnX += buttonWidth;
        }

        return buttonPanel;
    }

public:
    ReactorGUI(int width, int height, ReactorButtonTexturePack texturePack, std::function<void()> onReactorUpdate=nullptr, int reactorUpdateDelayMS=40): 
        Window(width, height),
        texturePack_(texturePack),
        reactorUpdateDelayMS_(reactorUpdateDelayMS)
    {
        reactorCanvasWidth_ = width - 2 * WINDOW_BORDER_SIZE;
        reactorCanvasHeight_ = (height - 3 * WINDOW_BORDER_SIZE) * REACTOR_CANVAS_SHARE;
        buttonPanelWidth_ = reactorCanvasWidth_;
        buttonPanelHeight_ = height - 3 * WINDOW_BORDER_SIZE - reactorCanvasHeight_;
    
        ReactorVisibleArea *reactorVisibleArea = new ReactorVisibleArea(reactorCanvasWidth_, reactorCanvasHeight_, this);
        reactorCanvas_ = new ReactorCanvas(reactorCanvasWidth_, reactorCanvasHeight_, onReactorUpdate, reactorVisibleArea);
        
        
        reactorVisibleArea->addWidget(0, 0, reactorCanvas_);
        reactorModel_ = reactorCanvas_->reactorModel();

        Container *ButtonPanel = createReactorButtonPanel(buttonPanelWidth_, buttonPanelHeight_);
    
        addWidget(WINDOW_BORDER_SIZE, WINDOW_BORDER_SIZE, reactorVisibleArea);
        addWidget(WINDOW_BORDER_SIZE, WINDOW_BORDER_SIZE * 2 + reactorCanvasHeight_, ButtonPanel);
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_SetRenderDrawColor(renderer, DEFAULT_WINDOW_COLOR.r, DEFAULT_WINDOW_COLOR.g, DEFAULT_WINDOW_COLOR.b, DEFAULT_WINDOW_COLOR.a); 
        SDL_Rect full = {0, 0, rect_.w, rect_.h};
        SDL_RenderFillRect(renderer, &full);
    }

    void setReactorOnUpdate(std::function<void()> updateFunc) { reactorModel_->setOnUpdate(updateFunc); }

    int getReactorCirclitCount() { return reactorModel_->getCirclitCount(); }
    int getReactorQuadritCount() { return reactorModel_->getQuadritCount(); }
    double getReactorSummaryEnergy() { return reactorModel_->getSummaryEnergy(); }

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