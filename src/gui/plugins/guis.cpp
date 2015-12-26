#include "guis.h"
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <cmath>
#include <QDebug>
#include <QObject>
#include "audio/core/plugins.h"

PluginGui::PluginGui(Audio::Plugin *plugin) :
    QWidget(0),
    plugin(plugin)
{
}

QString PluginGui::getPluginName() const
{
    return plugin->getName();
}

Audio::Plugin *PluginGui::getPlugin() const
{
    return plugin;
}

// ++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++
DelayGui::DelayGui(Audio::JamtabaDelay *delayPlugin) :
    PluginGui(delayPlugin)
{
    QGridLayout *mainLayout = new QGridLayout(this);

    // delay time
    sliderDelayTime = new QSlider(Qt::Horizontal, this);
    sliderDelayTime->setMinimum(1);
    sliderDelayTime->setMaximum(2000);// 2 seconds
    lineEditDelayTime = new QLineEdit(this);
    mainLayout->addWidget(new QLabel("Delay Time (ms):", this), 0, 0, Qt::AlignRight);
    mainLayout->addWidget(sliderDelayTime, 0, 1);
    mainLayout->addWidget(lineEditDelayTime, 0, 2);
    QObject::connect(sliderDelayTime, SIGNAL(sliderReleased()), this,
                     SLOT(on_sliderDelayReleased()));

    // feedback
    sliderFeedback = new QSlider(Qt::Horizontal, this);
    lineEditFeedback = new QLineEdit(this);
    mainLayout->addWidget(new QLabel("Feedback (db):", this), 1, 0, Qt::AlignRight);
    mainLayout->addWidget(sliderFeedback, 1, 1);
    mainLayout->addWidget(lineEditFeedback, 1, 2);
    QObject::connect(sliderFeedback, SIGNAL(sliderReleased()), this,
                     SLOT(on_sliderFeedbackReleased()));

    // level
    sliderLevel = new QSlider(Qt::Horizontal, this);// wet gain
    sliderLevel->setMaximum(100);
    lineEditLevel = new QLineEdit(this);
    mainLayout->addWidget(new QLabel("Level:", this), 2, 0, Qt::AlignRight);
    mainLayout->addWidget(sliderLevel, 2, 1);
    mainLayout->addWidget(lineEditLevel, 2, 2);
    QObject::connect(sliderLevel, SIGNAL(sliderReleased()), this, SLOT(on_sliderLevelReleased()));

    // initial values
    sliderDelayTime->setValue(sliderDelayTime->maximum() * 0.25);
    sliderFeedback->setValue(sliderFeedback->maximum() * 0.25);
    sliderLevel->setValue(sliderLevel->maximum() * 0.5);
}

// ++++++++++++++
void DelayGui::on_sliderDelayReleased()
{
    ((Audio::JamtabaDelay *)plugin)->setDelayTime(sliderDelayTime->value());
    lineEditDelayTime->setText(QString::number(sliderDelayTime->value()));
}

void DelayGui::on_sliderFeedbackReleased()
{
    int value = sliderFeedback->value();
    ((Audio::JamtabaDelay *)plugin)->setFeedback(value/100.0);
    float db = 20 * std::log10(value/100.0);
    lineEditFeedback->setText(QString::number(db, 'f', 1));
}

void DelayGui::on_sliderLevelReleased()
{
    int value = sliderLevel->value();
    ((Audio::JamtabaDelay *)plugin)->setLevel(value/100.0);
    lineEditLevel->setText(QString::number(value/100.0, 'f', 1));
}

// ++++++++++++++
DelayGui::~DelayGui()
{
    qDebug() << "detrutor delay GUI";
}
