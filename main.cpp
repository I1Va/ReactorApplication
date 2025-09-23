
#include "MyGUI.h"
#include "gm_primitives.hpp"
#include "ReactorModel.h"
#include "ReactorCanvas.h"


const int MAIN_WINDOW_WIDTH = 800;
const int MAIN_WINDOW_HEIGHT = 600;
const char MAIN_WINDOW_TITLE[] = "MyGUI";

const char PRESSED_BUTTON_IMAGE_PATH[] = "images/pressedButton.png";
const char UNPRESSED_BUTTON_IMAGE_PATH[] = "images/unpressedButton.png";

void ReactorUpdate() {
    std::cout << "Isolate Reactor has updated!\n";
}

void ReactorAddMolecule() {
    std::cout << "ReactorAddMolecule!\n";
}

int main(void) {

    MGApplication application;
    
    try {
        application.setMainWindow(MAIN_WINDOW_TITLE, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    } catch (const std::exception& e) {
        std::cerr << "FATAL : " << e.what() << "\n";
        return -1;
    }

    // ReactorModel reactor(100, 100, );

    SignalManager *signalManager = application.getSignalManager();
    MGMainWindow *mainWindow = application.getMainWindow();

    MGWindow *reactorWindow = mainWindow->addWindow(0, 0, 400, 400);
    MGWindow *window = mainWindow->addWindow(500, 0, 300, 300);
    

    ReactorCanvas *reactorCanvas = new ReactorCanvas(360, 360, std::nullopt, reactorWindow);
    reactorWindow->addWidget(reactorCanvas, {20, 20});

    MGButton *button = new MGButton(200, 200, PRESSED_BUTTON_IMAGE_PATH, UNPRESSED_BUTTON_IMAGE_PATH, [reactorCanvas](){ reactorCanvas->addCirclit(); }, window);
    window->addWidget(button, {0, 0});

    application.addEventToMainLoop([reactorCanvas](int frameDelay){ reactorCanvas->reactorUpdate(frameDelay); });

    application.run();
    return 0;
}

