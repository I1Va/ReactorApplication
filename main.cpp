
#include "MyGUI.h"
#include "gm_primitives.hpp"
#include "ReactorModel.h"
#include "ReactorGUI.h"
#include "Plots.h"

class RecorderWindow : public Window {
    RecorderWidget *recorder;

public:
    RecorderWindow(int height, int width, Widget *parent=nullptr): Window(height, width, parent) {
        recorder = new RecorderWidget(width - 2 * WINDOW_BORDER_SIZE, height - 2 * WINDOW_BORDER_SIZE, this);
        addWidget(WINDOW_BORDER_SIZE, WINDOW_BORDER_SIZE, recorder);
    }
};

int main() {
    UIManager application(800, 600);

    Container *mainWindow = new Container(780, 580);
    application.setMainWidget(10, 10, mainWindow);    

    ReactorGUI *reactorGUI = new ReactorGUI(20);
    mainWindow->addWidget(10, 10, reactorGUI);

    RecorderWindow *circlitRecorder = new RecorderWindow(100, 100, mainWindow);
    mainWindow->addWidget(400, 400, circlitRecorder);

    // for (int i = 0; i < 100; i++) {
    //     reactorGUI->addCirclit();
    // }

    


    application.addUserEvent([&reactorGUI](int deltaMS) { reactorGUI->updateReactor(deltaMS); });
    
    application.run();

    return 0;
}
