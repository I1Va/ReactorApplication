
#include "MyGUI.h"
#include "gm_primitives.hpp"
#include "ReactorModel.h"
#include "ReactorCanvas.h"


// const int MAIN_WINDOW_WIDTH = 800;
// const int MAIN_WINDOW_HEIGHT = 600;
// const char MAIN_WINDOW_TITLE[] = "MyGUI";

// const char NARROW_RIGHTWALL[] = "images/NarrowRightWall.png";
// const char NARROW_RIGHTWALL_PRESSED[] = "images/NarrowRightWallPressed.png";
// const char UNNARROW_RIGHTWALL[] = "images/UnNarrowRightWall.png";
// const char UNNARROW_RIGHTWALL_PRESSED[] = "images/UnNarrowRightWalPressed.png";

// const char ADD_CIRCLIT_PRESSED[] = "images/addCirclitPressed.png";
// const char ADD_CIRCLIT[] = "images/addCirclitUnPressed.png";
// const char ADD_QUADRIT_PRESSED[] = "images/addQuadritPressed.png";
// const char ADD_QUADRIT[] = "images/addQuadritUnPressed.png";
// const char REMOVE_MOLECULE[] = "images/removeMolecule.png";
// const char REMOVE_MOLECULE_PRESSED[] = "images/removeMoleculePressed.png";

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

class Button : public Widget {
    
public:
    Button(int x, int y, int w, int h) : Widget(x, y, w, h) {}    


    void paintEvent(SDL_Renderer* renderer) {
        assert(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
        SDL_Rect full = {0, 0, rect_.w, rect_.h};
        SDL_RenderFillRect(renderer, &full);
    }
};

class Window : public Container {
public:
    Window(int x, int y, int w, int h) : Container(x, y, w, h) {}    


    void paintEvent(SDL_Renderer* renderer) {
        assert(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
        SDL_Rect full = {0, 0, rect_.w, rect_.h};
        SDL_RenderFillRect(renderer, &full);
    }
};




int main() {
    UIManager application(800, 600);

    Window *window = new Window(100, 100, 600, 400);

    // Container *subWindow1 = new Container(100, 100, 100, 100);

    // Window *subWindow2 = new Window(30, 30, 30, 30);


    // window->addWdiget(subWindow1);
    // subWindow1->addWdiget(subWindow2);


    
    // for (int i = 0; i < 5; i++) {
    //     Button *w1 = new Button(100 * i, 100 * i, 100, 100);
    //     window->addWdiget(w1);
    // }
    




    application.setMainWidget(window);



    application.run();



    return 0;
}
