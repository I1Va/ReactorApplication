
#include "MyGUI.h"
#include "gm_primitives.hpp"
#include "ReactorModel.h"
#include "ReactorGUI.h"
#include "Plots.h"
#include "ClockWidget.h"

const SDL_Color ENERGY_COLOR = {0, 200, 255, 255};

const int APP_BORDER_SZ = 10;
const gm_dot<int, 2> MAIN_WINDOW_SZ = {800, 600};
const gm_dot<int, 2> REACTOR_GUI_SZ = {300, 500};
const gm_dot<int, 2> PLOT_SZ = {(REACTOR_GUI_SZ.y - APP_BORDER_SZ) / 2, (REACTOR_GUI_SZ.y - APP_BORDER_SZ) / 2};
const int CLOCK_WINDOW_LENGTH = 200;

const char FONT_PATH[] = "fonts/Roboto/RobotoFont.ttf";


class RecorderWindow : public Window {
    RecorderWidget *recorder_;

public:
    RecorderWindow(int height, int width, Widget *parent=nullptr): Window(height, width, parent) {
        recorder_ = new RecorderWidget(width - 2 * WINDOW_BORDER_SIZE, height - 2 * WINDOW_BORDER_SIZE, this);
        addWidget(WINDOW_BORDER_SIZE, WINDOW_BORDER_SIZE, recorder_);
    }

    void addPoint(double y, SDL_Color color) { 
        recorder_->addPoint(y, color); 
        recorder_->setRerenderFlag();
    }

    void endRecord() { recorder_->endRecord(); }
};

int main() {
    UIManager application(MAIN_WINDOW_SZ.x, MAIN_WINDOW_SZ.y);

    Container *mainWindow = new Container(MAIN_WINDOW_SZ.x - 2 * APP_BORDER_SZ, MAIN_WINDOW_SZ.y - 2 * APP_BORDER_SZ);
    application.setMainWidget(APP_BORDER_SZ, APP_BORDER_SZ, mainWindow);    

    RecorderWindow *moleculesRecorder = new RecorderWindow(PLOT_SZ.x, PLOT_SZ.y, mainWindow);
    mainWindow->addWidget(REACTOR_GUI_SZ.x + 2 * APP_BORDER_SZ, APP_BORDER_SZ, moleculesRecorder);

    RecorderWindow *energyRecorder = new RecorderWindow(PLOT_SZ.x, PLOT_SZ.y, mainWindow);
    mainWindow->addWidget(REACTOR_GUI_SZ.x + 2 * APP_BORDER_SZ, PLOT_SZ.y + 2 * APP_BORDER_SZ, energyRecorder);

    ReactorGUI *reactorGUI = new ReactorGUI(REACTOR_GUI_SZ.x, REACTOR_GUI_SZ.y, nullptr, 40);
    reactorGUI->setReactorOnUpdate(
        [reactorGUI, moleculesRecorder, energyRecorder] {
            int reactorCirclitCount = reactorGUI->getReactorCirclitCount();
            int reactorQuadritCount = reactorGUI->getReactorQuadritCount();
            double reactorEnergy = reactorGUI->getReactorSummaryEnergy();

            moleculesRecorder->addPoint(reactorCirclitCount, RED_SDL_COLOR);
            moleculesRecorder->addPoint(reactorQuadritCount, BLUE_SDL_COLOR);
            moleculesRecorder->endRecord();

            energyRecorder->addPoint(reactorEnergy, ENERGY_COLOR);
            energyRecorder->endRecord();
        }
    );
    mainWindow->addWidget(APP_BORDER_SZ, APP_BORDER_SZ, reactorGUI);


    ClockWindow *clockWindow = new ClockWindow(CLOCK_WINDOW_LENGTH, FONT_PATH, mainWindow);
    mainWindow->addWidget(3 * APP_BORDER_SZ + REACTOR_GUI_SZ.x + PLOT_SZ.x, APP_BORDER_SZ, clockWindow);

  
    application.addUserEvent([&reactorGUI](int deltaMS) { reactorGUI->updateReactor(deltaMS); });
    
    application.run();

    return 0;
}
