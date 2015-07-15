#include "LocalTrackGroupView.h"
#include "ui_LocalTrackGroupView.h"
#include "LocalTrackView.h"
#include <QPainter>
#include <QMenu>
#include "Highligther.h"

LocalTrackGroupView::LocalTrackGroupView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalTrackGroupView)
{
    ui->setupUi(this);

    //ui->tracksPanel->setLayout(new QHBoxLayout(ui->tracksPanel));
    ui->tracksPanel->layout()->setContentsMargins(0, 0, 0, 0);
    ui->tracksPanel->layout()->setSpacing(2);
}

void LocalTrackGroupView::updatePeaks(){
    foreach (LocalTrackView* trackView, trackViews) {
        trackView->updatePeaks();
    }
}

LocalTrackGroupView::~LocalTrackGroupView()
{
    delete ui;
}

//little to allow stylesheet in custom widget
void LocalTrackGroupView::paintEvent(QPaintEvent* ){
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LocalTrackGroupView::addTrackView(LocalTrackView *trackView){
    if(trackViews.size() >= MAX_SUB_CHANNELS){
        return;
    }
    if(ui->tracksPanel->layout()){
        ui->tracksPanel->layout()->addWidget(trackView);
        trackViews.append(trackView);

        if(trackViews.size() > 1){
            foreach(LocalTrackView* trackView, trackViews){
                trackView->setToNarrow();
            }
            updateGeometry();
        }
    }
}

void LocalTrackGroupView::setGroupName(QString groupName){
    ui->groupNameField->setText(groupName);
}

QString LocalTrackGroupView::getGroupName() const{
    return ui->groupNameField->text();
}

void LocalTrackGroupView::on_toolButton_clicked()
{
    QMenu menu;
    QAction* addSubchannelAction = menu.addAction(QIcon(":/images/more.png"), "Add subchannel");
    QObject::connect(addSubchannelAction, SIGNAL(triggered()), this, SLOT(onAddSubChannelClicked()));
    addSubchannelAction->setEnabled(trackViews.size() < MAX_SUB_CHANNELS);
    if(trackViews.size() > 1 ){
        menu.addSeparator();
        for (int i = 2; i <= trackViews.size(); ++i) {
            QAction* action = menu.addAction(QIcon(":/images/less.png"), "Remove subchannel " + QString::number(i));
            action->setData( i-1 );//use track view index as user data
        }
    }
    QObject::connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(on_toolButtonActionTriggered(QAction*)));
    QObject::connect(&menu, SIGNAL(hovered(QAction*)), this, SLOT(on_toolButtonActionHovered(QAction*)));
    menu.move( mapToGlobal( ui->toolButton->pos() + QPoint(ui->toolButton->width(), 0) ));
    menu.exec();
}

void LocalTrackGroupView::onAddSubChannelClicked(){
    addTrackView(new LocalTrackView(this, trackViews.at(0)->getMainController()));
}

void LocalTrackGroupView::on_toolButtonActionHovered(QAction *action){
    if(action->data().isValid()){//only remove actions contains data
        int trackIndex = action->data().toInt();
        if(trackIndex >= 0 && trackIndex < trackViews.size()){
            Highligther::getInstance()->highlight(trackViews.at(trackIndex));
        }
    }
}

void LocalTrackGroupView::on_toolButtonActionTriggered(QAction *action){
    if(action->data().isValid()){//only remove actions contains data
        int trackIndex = action->data().toInt();
        if(trackIndex >= 0 && trackIndex < trackViews.size()){
            LocalTrackView* trackView = trackViews.at(trackIndex);
            ui->tracksPanel->layout()->removeWidget(trackView);
            trackViews.removeAt(trackIndex);
            trackView->deleteLater();

            if(trackViews.size() == 1){
                trackViews.at(0)->setToWide();
            }

            updateGeometry();
        }
    }
}

QSize LocalTrackGroupView::minimumSizeHint() const{
    return sizeHint();
}

QSize LocalTrackGroupView::sizeHint() const{
    if(trackViews.size() > 1){
        return QSize(trackViews.size() * BaseTrackView::NARROW_WIDTH, 10);
    }
    return trackViews.at(0)->sizeHint();
}
