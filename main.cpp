
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

public:
    ReactorGUI(): Window(REACTOR_GUI_WIDTH, REACTOR_GUI_HEIGHT) {
        int buttonStartY = REACTOR_GUI_HEIGHT - BUTTON_PANEL_HEIGHT;
        Button *addCirclitBtn = new Button(BUTTON_SIZE, BUTTON_SIZE, ADD_CIRCLIT, ADD_CIRCLIT_PRESSED, nullptr, this);

        ReactorCanvas *reactor = new ReactorCanvas(REACTOR_WIDTH, REACTOR_HEIGHT, std::nullopt, 40, this);
        addWidget(BORDER_SIZE, BORDER_SIZE, reactor);

        addWidget(BORDER_SIZE, buttonStartY, addCirclitBtn);
    }

    void renderSelfAction(SDL_Renderer* renderer) override {
        assert(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
        SDL_Rect full = {0, 0, rect_.w, rect_.h};
        SDL_RenderFillRect(renderer, &full);
    }
};

// Button
//     (
//         int x, int y, int w, int h,
//         const char *unpressedButtonTexturePath, const char *pressedButtonTexturePath,
//         std::function<void()> onClickFunction=nullptr, Widget *parent=nullptr
//     ): 


int main() {
    UIManager application(800, 600);

    Container *mainWindow = new Container(780, 580);
    application.setMainWidget(10, 10, mainWindow);    

    ReactorGUI *reactor = new ReactorGUI();

    mainWindow->addWidget(10, 10, reactor);
    application.run();

    return 0;
}
