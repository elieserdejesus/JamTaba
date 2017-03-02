#ifndef LOOPERWINDOW_H
#define LOOPERWINDOW_H

#include <QDialog>
#include <QCheckBox>
#include <QMap>
#include <QLayout>

#include "audio/looper/AudioLooper.h"
#include "LooperWavePanel.h"

namespace Controller {
class NinjamController;
}

namespace Ui {
class LooperWindow;
}

class LooperWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LooperWindow(QWidget *parent);
    ~LooperWindow();

    void setLooper(Audio::Looper *looper, Controller::NinjamController *controller);
    void detachCurrentLooper();
    void updateDrawings();

protected:
    void paintEvent(QPaintEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;

private slots:
    void updateBeatsPerInterval();
    void updateCurrentBeat(uint currentIntervalBeat);
    void updateControls();
    void updateLayersVisibility(quint8 newMaxLayers);

private:
    Ui::LooperWindow *ui;
    Audio::Looper *looper;
    QMap<quint8, LooperWavePanel*> wavePanels;
    Controller::NinjamController *controller;

    void deleteWavePanels();

    QLayout *createLayerControls(Audio::Looper *looper, quint8 layerIndex);

    void addSamplesPeak(float peak, uint samplesCount, quint8 layerIndex);

    void initializeControls();

    void createRecordingOptionsCheckBoxes();
    void createPlayingOptionsCheckBoxes();

    QMap<Audio::Looper::RecordingOption, QCheckBox *> recordingCheckBoxes;
    QMap<Audio::Looper::PlayingOption, QCheckBox *> playingCheckBoxes;

    template<class OptionType>
    void createOptionsCheckBoxes(QLayout *layoutToAddCheckboxes, const QList<OptionType> &options, QMap<OptionType, QCheckBox *> &mapToAddCheckBoxes)
    {
        clearLayout(layoutToAddCheckboxes);
        mapToAddCheckBoxes.clear();

        for (auto option : options) {
            QCheckBox *newCheckBox = new QCheckBox(getOptionName(option));
            newCheckBox->setToolTip(getOptionToolTip(option));
            newCheckBox->setProperty("option", qVariantFromValue(option));
            mapToAddCheckBoxes.insert(option, newCheckBox);

            layoutToAddCheckboxes->addWidget(newCheckBox);

            connect(newCheckBox, &QCheckBox::toggled, [=](bool checked){
                if (looper)
                    looper->setOption(option, checked);
            });
        }
    }


    void clearLayout(QLayout *layout);

    static QString getOptionName(Audio::Looper::RecordingOption option);
    static QString getOptionName(Audio::Looper::PlayingOption option);

    static QString getOptionToolTip(Audio::Looper::RecordingOption option);
    static QString getOptionToolTip(Audio::Looper::PlayingOption option);

    static QList<Audio::Looper::RecordingOption> getAllRecordingOptions();
    static QList<Audio::Looper::PlayingOption> getAllPlayingOptions();

    template<class OptionType>
    void updateOptions(QLayout *layout)
    {
        for (int i = 0; i < layout->count(); ++i) {
            QCheckBox *checkBox = qobject_cast<QCheckBox *>(layout->itemAt(i)->widget());
            if (checkBox) {
                OptionType option = qvariant_cast<OptionType>(checkBox->property("option"));
                bool optionIsSupported = looper->optionIsSupportedInCurrentMode(option);
                checkBox->setEnabled(optionIsSupported);
                checkBox->setChecked(optionIsSupported && looper->getOption(option));
            }
            else{
                qCritical() << "Checkbox is null!";
            }
        }
    }

    int currentBeat;
};

Q_DECLARE_METATYPE(Audio::Looper::RecordingOption)
Q_DECLARE_METATYPE(Audio::Looper::PlayingOption)

#endif // LOOPERWINDOW_H
