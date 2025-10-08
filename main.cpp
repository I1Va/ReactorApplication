
#include "MyGUI.h"
#include "gm_primitives.hpp"
#include "ReactorModel.h"
#include "ReactorGUI.h"
#include "Plots.h"
#include "ClockWidget.h"
#include "ScrollBar.h"

const SDL_Color ENERGY_COLOR = {0, 200, 255, 255};

const int APP_BORDER_SZ = 10;
const gm_dot<int, 2> MAIN_WINDOW_SZ = {800, 600};
const gm_dot<int, 2> REACTOR_GUI_SZ = {300, 500};
const gm_dot<int, 2> PLOT_SZ = {(REACTOR_GUI_SZ.y - APP_BORDER_SZ) / 2, (REACTOR_GUI_SZ.y - APP_BORDER_SZ) / 2};
const int CLOCK_WINDOW_LENGTH = 200;
const gm_dot<int, 2> SCROLL_BAR_SZ = {100, 20};

const char FONT_PATH[] = "fonts/Roboto/RobotoFont.ttf";

const ReactorButtonTexturePack reactorButtonTexturePack = 
{
    .narrowRightWallBtnPath     = {"images/reactorButtonPanel/narrowRightWall/unpressed.png", "images/reactorButtonPanel/narrowRightWall/pressed.png"},
    .unNarrowRightWallBtnPath   = {"images/reactorButtonPanel/unnarowRightWall/unpressed.png", "images/reactorButtonPanel/unnarowRightWall/pressed.png"},
    .addCirclitBtnPath          = {"images/reactorButtonPanel/addCirclit/unpressed.png", "images/reactorButtonPanel/addCirclit/pressed.png"},
    .addQuadritBtnPath          = {"images/reactorButtonPanel/addQuadrit/unpressed.png", "images/reactorButtonPanel/addQuadrit/pressed.png"},
    .removeMoleculeBtnPath      = {"images/reactorButtonPanel/removeMolecule/unpressed.png", "images/reactorButtonPanel/removeMolecule/pressed.png"},

    .heatTopWallBtnPath         = {"images/reactorButtonPanel/heatTopWall/unpressed.png", "images/reactorButtonPanel/heatTopWall/pressed.png"},
    .heatBottomWallBtnPath      = {"images/reactorButtonPanel/heatBottomWall/unpressed.png", "images/reactorButtonPanel/heatBottomWall/pressed.png"},
    .heatLeftWallBtnPath        = {"images/reactorButtonPanel/heatLeftWall/unpressed.png", "images/reactorButtonPanel/heatLeftWall/pressed.png"},
    .heatRightWallBtnPath       = {"images/reactorButtonPanel/hearRightWall/unpressed.png", "images/reactorButtonPanel/hearRightWall/pressed.png"},

    .explodeReactorBtnPath      = {"images/reactorButtonPanel/explode/unpressed.png", "images/reactorButtonPanel/explode/pressed.png"}
};

int main() {
    UIManager application(MAIN_WINDOW_SZ.x, MAIN_WINDOW_SZ.y);

    Container *mainWindow = new Container(MAIN_WINDOW_SZ.x - 2 * APP_BORDER_SZ, MAIN_WINDOW_SZ.y - 2 * APP_BORDER_SZ);
    application.setMainWidget(APP_BORDER_SZ, APP_BORDER_SZ, mainWindow);    

    RecorderWindow *moleculesRecorder = new RecorderWindow(PLOT_SZ.x, PLOT_SZ.y, mainWindow);
    mainWindow->addWidget(REACTOR_GUI_SZ.x + 2 * APP_BORDER_SZ, APP_BORDER_SZ, moleculesRecorder);

    RecorderWindow *energyRecorder = new RecorderWindow(PLOT_SZ.x, PLOT_SZ.y, mainWindow);
    mainWindow->addWidget(REACTOR_GUI_SZ.x + 2 * APP_BORDER_SZ, PLOT_SZ.y + 2 * APP_BORDER_SZ, energyRecorder);

    ReactorGUI *reactorGUI = new ReactorGUI(REACTOR_GUI_SZ.x, REACTOR_GUI_SZ.y, reactorButtonTexturePack, nullptr, 40);
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

  
    ScrollBar *scrollBar = new ScrollBar(SCROLL_BAR_SZ.x, SCROLL_BAR_SZ.y, nullptr, true, mainWindow);
    mainWindow->addWidget(600, 500, scrollBar);



    
    application.addUserEvent([reactorGUI, clockWindow](int deltaMS) { 
        reactorGUI->updateReactor(deltaMS); 
        clockWindow->updateClock(deltaMS);
    });
    
    application.run();

    return 0;
}
