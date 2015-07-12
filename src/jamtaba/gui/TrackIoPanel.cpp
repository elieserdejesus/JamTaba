#include "TrackIOPanel.h"
#include "ui_TrackIOPanel.h"

TrackIOPanel::TrackIOPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackIOPanel)
{
    ui->setupUi(this);
}

TrackIOPanel::~TrackIOPanel()
{
    delete ui;
}
