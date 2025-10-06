
#include "MyGUI.h"
#include "gm_primitives.hpp"
#include "ReactorModel.h"
#include "ReactorCanvas.h"

// const int MAIN_WINDOW_WIDTH = 800;
// const int MAIN_WINDOW_HEIGHT = 600;
// const char MAIN_WINDOW_TITLE[] = "MyGUI";

// const size_t BUTTON_SZ = 50;
// const size_t REACTOR_SZ = 300;
// const size_t REACTOR_BORDER = 30;

// int main(void) {

//     MGApplication application;
    
//     try {
//         application.setMainWindow(MAIN_WINDOW_TITLE, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
//     } catch (const std::exception& e) {
//         std::cerr << "FATAL : " << e.what() << "\n";
//         return -1;
//     }

//     SignalManager *signalManager = application.getSignalManager();
//     MGMainWindow *mainWindow = application.getMainWindow();
    
//     MGWindow *reactorWindow = mainWindow->addWindow(0, 0, 800, 600);
    
//     // MGWindow *window = mainWindow->addWindow(200, 200, 400, 400);   


//     ReactorCanvas *reactorCanvas = new ReactorCanvas(REACTOR_SZ, REACTOR_SZ, std::nullopt, reactorWindow);
//     for (size_t i = 0; i < 400; i++) {
//         reactorCanvas->addCirclit();
//     }

//     reactorWindow->addWidget(reactorCanvas, {REACTOR_BORDER, REACTOR_BORDER});

//     MGButton *addCirclitBtn = new MGButton(BUTTON_SZ, BUTTON_SZ, ADD_CIRCLIT, ADD_CIRCLIT_PRESSED,
//                                            [reactorCanvas](){ reactorCanvas->addCirclit(); }, reactorWindow);
                                        
//     MGButton *addQuadrittBtn = new MGButton(BUTTON_SZ, BUTTON_SZ, ADD_QUADRIT, ADD_QUADRIT_PRESSED, 
//                                             [reactorCanvas](){ reactorCanvas->addQuadrit(); }, reactorWindow);
                                        
//     MGButton *removeMoleculeBtn = new MGButton(BUTTON_SZ, BUTTON_SZ, REMOVE_MOLECULE, REMOVE_MOLECULE_PRESSED, 
//                                                [reactorCanvas](){ reactorCanvas->removeMolecule(); }, reactorWindow);
                                            
//     MGButton *narrowRightWallBtn = new MGButton(BUTTON_SZ, BUTTON_SZ, NARROW_RIGHTWALL, NARROW_RIGHTWALL_PRESSED, 
//                                                 [reactorCanvas](){ reactorCanvas->narrowRightWall(); }, reactorWindow);

//     MGButton *unNarrowRightWallBtn = new MGButton(BUTTON_SZ, BUTTON_SZ, UNNARROW_RIGHTWALL, UNNARROW_RIGHTWALL_PRESSED, 
//                                                 [reactorCanvas](){ reactorCanvas->unNarrowRightWall(); }, reactorWindow);


//     reactorWindow->addWidget(addCirclitBtn, {REACTOR_SZ + REACTOR_BORDER, REACTOR_BORDER});
//     reactorWindow->addWidget(addQuadrittBtn, {REACTOR_SZ + REACTOR_BORDER + BUTTON_SZ, REACTOR_BORDER});
//     reactorWindow->addWidget(removeMoleculeBtn, {REACTOR_SZ + REACTOR_BORDER + BUTTON_SZ * 2, REACTOR_BORDER});
//     reactorWindow->addWidget(narrowRightWallBtn, {REACTOR_SZ + REACTOR_BORDER, REACTOR_BORDER + BUTTON_SZ});
//     reactorWindow->addWidget(unNarrowRightWallBtn, {REACTOR_SZ + REACTOR_BORDER + BUTTON_SZ, REACTOR_BORDER + BUTTON_SZ});
    
//     application.addEventToMainLoop([reactorCanvas](int frameDelay){ reactorCanvas->reactorUpdate(frameDelay); });

//     application.run();
//     return 0;
// }



// void renderSelfAction(SDL_Renderer* renderer) {
//     assert(renderer);

//     SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
//     SDL_Rect full = {0, 0, rect_.w, rect_.h};
//     SDL_RenderFillRect(renderer, &full);
// }

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

class ReactorCanvas : public Container {
    bool needReCalc_ = false;
    const ReactorModel& reactorModel_;
    
    

    // visible area of ​​the rector and walls
    // wall1, wall2, wall3, wall4 
    std::vector<MGShape *> geomPrimitives_ = {}; // molecules

public:
    ReactorCanvas
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

class ReactorGUI : public Window {
    static constexpr const int BORDER_SIZE = 10;
    static constexpr const int REACTOR_GUI_HEIGHT = 400;
    static constexpr const int REACTOR_GUI_WIDTH = 350;

    static constexpr const int REACTOR_HEIGHT = 300;
    static constexpr const int REACTOR_WIDTH = REACTOR_GUI_WIDTH - 2 * BORDER_SIZE;
    static constexpr const int BUTTON_PANEL_HEIGHT = REACTOR_GUI_HEIGHT - REACTOR_HEIGHT - 2 * BORDER_SIZE;
    static constexpr const int BUTTON_SIZE = 50;

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


public:
    ReactorGUI(int reactorUpdateDelayMS=40): 
        Window(REACTOR_GUI_WIDTH, REACTOR_GUI_HEIGHT),
        reactorUpdateDelayMS_(reactorUpdateDelayMS),
        reactorModel_(REACTOR_WIDTH, REACTOR_HEIGHT, nullptr, std::nullopt) 
    {
        int buttonStartY = REACTOR_GUI_HEIGHT - BUTTON_PANEL_HEIGHT;
        Button *addCirclitBtn = new Button(BUTTON_SIZE, BUTTON_SIZE, ADD_CIRCLIT, ADD_CIRCLIT_PRESSED, nullptr, this);

        reactorCanvas_ = new ReactorCanvas(REACTOR_WIDTH, REACTOR_HEIGHT, reactorModel_, this);
        addWidget(BORDER_SIZE, BORDER_SIZE, reactorCanvas_);

        addWidget(BORDER_SIZE, buttonStartY, addCirclitBtn);
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

int main() {
    UIManager application(800, 600);

    Container *mainWindow = new Container(780, 580);
    application.setMainWidget(10, 10, mainWindow);    

    ReactorGUI *reactorGUI = new ReactorGUI(20);
    mainWindow->addWidget(10, 10, reactorGUI);

    for (int i = 0; i < 100; i++) {
        reactorGUI->addCirclit();
    }


    application.addUserEvent([&reactorGUI](int deltaMS) { reactorGUI->updateReactor(deltaMS); });
    
    application.run();

    return 0;
}
