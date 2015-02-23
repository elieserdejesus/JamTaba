#include "guis.h"
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include "../audio/core/plugins.h"

PluginGui::PluginGui(Audio::Plugin *plugin)
    :QWidget(0), plugin(plugin)
{

}
QString PluginGui::getPluginName() const{
    return plugin->getName();
}

//++++++++++++++++++++++++++++++++++++++++++++++
DelayGui::DelayGui(Plugin::JamtabaDelay* delayPlugin)
    :PluginGui(delayPlugin)
{
    QGridLayout* mainLayout = new QGridLayout(this);

    //delay time
    QSlider* sliderDelayTime = new QSlider(Qt::Horizontal,this);
    QLineEdit* lineEditDelayTime = new QLineEdit(this);
    mainLayout->addWidget(new QLabel("Delay Time:", this), 0, 0, Qt::AlignRight);
    mainLayout->addWidget(sliderDelayTime, 0, 1);
    mainLayout->addWidget(lineEditDelayTime, 0, 2);

    //feedback
    QSlider* sliderFeedback = new QSlider(Qt::Horizontal, this);
    QLineEdit* lineEditFeedback = new QLineEdit(this);
    mainLayout->addWidget(new QLabel("Feedback:", this), 1, 0, Qt::AlignRight);
    mainLayout->addWidget(sliderFeedback, 1, 1);
    mainLayout->addWidget(lineEditFeedback, 1, 2);

    //level
    QSlider* sliderLevel = new QSlider(Qt::Horizontal,this);//wet gain
    QLineEdit* lineEditLevel = new QLineEdit(this);
    mainLayout->addWidget(new QLabel("Level:", this), 2, 0, Qt::AlignRight);
    mainLayout->addWidget(sliderLevel, 2, 1);
    mainLayout->addWidget(lineEditLevel, 2, 2);
}

DelayGui::~DelayGui()
{
    qDebug() << "detrutor delay GUI";
}

