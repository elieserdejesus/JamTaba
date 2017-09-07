#include "MainWindowVST.h"

#include "MainControllerVST.h"
#include "SonarTextEditorModifier.h"

MainWindowVST::MainWindowVST(MainControllerVST *controller)
    : MainWindowPlugin(controller)
{

}

TextEditorModifier *MainWindowVST::createTextEditorModifier()
{
    auto controller = static_cast<MainControllerVST *>(mainController);
    bool hostIsSonar = controller->getHostName().contains("sonar", Qt::CaseInsensitive);
    if (hostIsSonar)
        return new SonarTextEditorModifier();

    return MainWindowPlugin::createTextEditorModifier();
}
