
#include "MyGUI.h"
#include "gm_primitives.hpp"
#include "ReactorModel.h"
#include "ReactorGUI.h"
#include "Plots.h"

const SDL_Color ENERGY_COLOR = {0, 200, 255, 255};

class RecorderWindow : public Window {
    RecorderWidget *recorder_;

public:
    RecorderWindow(int height, int width, Widget *parent=nullptr): Window(height, width, parent) {
        recorder_ = new RecorderWidget(width - 2 * WINDOW_BORDER_SIZE, height - 2 * WINDOW_BORDER_SIZE, this);
        addWidget(WINDOW_BORDER_SIZE, WINDOW_BORDER_SIZE, recorder_);
    }

    void addPoint(double y, SDL_Color color) { recorder_->addPoint(y, color); }
    void endRecord() { recorder_->endRecord(); }
};

int main() {
    UIManager application(800, 600);

    Container *mainWindow = new Container(780, 580);
    application.setMainWidget(10, 10, mainWindow);    

    RecorderWindow *moleculesRecorder = new RecorderWindow(300, 300, mainWindow);
    mainWindow->addWidget(400, 0, moleculesRecorder);

    RecorderWindow *energyRecorder = new RecorderWindow(300, 300, mainWindow);
    mainWindow->addWidget(400, 300, energyRecorder);



    ReactorGUI *reactorGUI = new ReactorGUI(nullptr, 40);
    reactorGUI->setReactorOnUpdate(
        [reactorGUI, moleculesRecorder, energyRecorder] {
            int reactorCirclitCount = reactorGUI->getReactorCirclitCount();
            int reactorQuadritCount = reactorGUI->getReactorQuadritCount();
            double reactorEnergy = reactorGUI->getReactorSummaryEnergy();
            
            moleculesRecorder->addPoint(reactorCirclitCount, RED_SDL_COLOR);
            moleculesRecorder->addPoint(reactorQuadritCount, BLUE_SDL_COLOR);
            moleculesRecorder->endRecord();
            moleculesRecorder->setRerenderFlag();

            energyRecorder->addPoint(reactorEnergy, ENERGY_COLOR);
            energyRecorder->endRecord();
            energyRecorder->setRerenderFlag();
        }
    );


    mainWindow->addWidget(10, 10, reactorGUI);

   

    // for (int i = 0; i < 100; i++) {
    //     reactorGUI->addCirclit();
    // }

    


    application.addUserEvent([&reactorGUI](int deltaMS) { reactorGUI->updateReactor(deltaMS); });
    
    application.run();

    return 0;
}
