#include "LocalTrackView.h"
#include "ui_trackview.h"
#include "FxPanel.h"
#include "FxPanelItem.h"
#include "ui_fxpanel.h"
#include "plugins/guis.h"

LocalTrackView::LocalTrackView(QWidget* parent)
    :TrackView(parent), fxPanel(nullptr)
{

}

void LocalTrackView::addPlugin(PluginGui *pluginGui){
    if(fxPanel){
        this->fxPanel->addPlugin(pluginGui);
    }
}

void LocalTrackView::initializeFxPanel(QMenu *fxMenu){
    fxPanel = new FxPanel(this, fxMenu);
    fxPanel->connect(fxPanel, SIGNAL(editingPlugin(PluginGui*)), this, SIGNAL(editingPlugin(PluginGui*)));
    int index = ui->mainLayout->indexOf(ui->panSectionSeparator);

    ui->mainLayout->insertWidget( index+1, fxPanel);

    //add separator after effects panel
    QFrame* separator = new QFrame(this);
    separator->setObjectName(QStringLiteral("FX separator"));
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    ui->mainLayout->insertWidget(index + 2, separator);
}

LocalTrackView::~LocalTrackView()
{

}

