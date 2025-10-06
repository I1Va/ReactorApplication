
#include "MyGUI.h"
#include "gm_primitives.hpp"
#include "ReactorModel.h"
#include "ReactorGUI.h"

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


//     ReactorView *ReactorView = new ReactorView(REACTOR_SZ, REACTOR_SZ, std::nullopt, reactorWindow);
//     for (size_t i = 0; i < 400; i++) {
//         ReactorView->addCirclit();
//     }

//     reactorWindow->addWidget(ReactorView, {REACTOR_BORDER, REACTOR_BORDER});

//     MGButton *addCirclitBtn = new MGButton(BUTTON_SZ, BUTTON_SZ, ADD_CIRCLIT, ADD_CIRCLIT_PRESSED,
//                                            [ReactorView](){ ReactorView->addCirclit(); }, reactorWindow);
                                        
//     MGButton *addQuadrittBtn = new MGButton(BUTTON_SZ, BUTTON_SZ, ADD_QUADRIT, ADD_QUADRIT_PRESSED, 
//                                             [ReactorView](){ ReactorView->addQuadrit(); }, reactorWindow);
                                        
//     MGButton *removeMoleculeBtn = new MGButton(BUTTON_SZ, BUTTON_SZ, REMOVE_MOLECULE, REMOVE_MOLECULE_PRESSED, 
//                                                [ReactorView](){ ReactorView->removeMolecule(); }, reactorWindow);
                                            
//     MGButton *narrowRightWallBtn = new MGButton(BUTTON_SZ, BUTTON_SZ, NARROW_RIGHTWALL, NARROW_RIGHTWALL_PRESSED, 
//                                                 [ReactorView](){ ReactorView->narrowRightWall(); }, reactorWindow);

//     MGButton *unNarrowRightWallBtn = new MGButton(BUTTON_SZ, BUTTON_SZ, UNNARROW_RIGHTWALL, UNNARROW_RIGHTWALL_PRESSED, 
//                                                 [ReactorView](){ ReactorView->unNarrowRightWall(); }, reactorWindow);


//     reactorWindow->addWidget(addCirclitBtn, {REACTOR_SZ + REACTOR_BORDER, REACTOR_BORDER});
//     reactorWindow->addWidget(addQuadrittBtn, {REACTOR_SZ + REACTOR_BORDER + BUTTON_SZ, REACTOR_BORDER});
//     reactorWindow->addWidget(removeMoleculeBtn, {REACTOR_SZ + REACTOR_BORDER + BUTTON_SZ * 2, REACTOR_BORDER});
//     reactorWindow->addWidget(narrowRightWallBtn, {REACTOR_SZ + REACTOR_BORDER, REACTOR_BORDER + BUTTON_SZ});
//     reactorWindow->addWidget(unNarrowRightWallBtn, {REACTOR_SZ + REACTOR_BORDER + BUTTON_SZ, REACTOR_BORDER + BUTTON_SZ});
    
//     application.addEventToMainLoop([ReactorView](int frameDelay){ ReactorView->reactorUpdate(frameDelay); });

//     application.run();
//     return 0;
// }



// void renderSelfAction(SDL_Renderer* renderer) {
//     assert(renderer);

//     SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
//     SDL_Rect full = {0, 0, rect_.w, rect_.h};
//     SDL_RenderFillRect(renderer, &full);
// }


int main() {
    UIManager application(800, 600);

    Container *mainWindow = new Container(780, 580);
    application.setMainWidget(10, 10, mainWindow);    

    ReactorGUI *reactorGUI = new ReactorGUI(20);
    mainWindow->addWidget(10, 10, reactorGUI);

    // for (int i = 0; i < 100; i++) {
    //     reactorGUI->addCirclit();
    // }


    application.addUserEvent([&reactorGUI](int deltaMS) { reactorGUI->updateReactor(deltaMS); });
    
    application.run();

    return 0;
}
