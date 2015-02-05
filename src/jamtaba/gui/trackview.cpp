#include "trackview.h"
#include "ui_trackview.h"
#include <QStyleOption>
#include <QPainter>


TrackView::TrackView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackView)
{
    ui->setupUi(this);
}

TrackView::~TrackView()
{
    delete ui;
}

//little to allow stylesheet in custom widget
void TrackView::paintEvent(QPaintEvent* ){
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
