#ifndef LOOPERWINDOW_H
#define LOOPERWINDOW_H

#include <QDialog>
#include <QCheckBox>
#include <QMap>
#include <QLayout>

#include "looper/Looper.h"
#include "LooperWavePanel.h"
#include "looper/LooperPersistence.h"

namespace Controller {
class NinjamController;
class MainController;
}

namespace Ui {
class LooperWindow;
}

class LooperWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LooperWindow(QWidget *parent, Controller::MainController *mainController);
    ~LooperWindow();

    void setLooper(Audio::Looper *looper);
    void detachCurrentLooper();
    void updateDrawings();

protected:
    void paintEvent(QPaintEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
    bool eventFilter(QObject *source, QEvent *ev);

private slots:
    void updateBeatsPerInterval();
    void updateCurrentBeat(uint currentIntervalBeat);
    void handleNewMaxLayers(quint8 newMaxLayers);
    void handleModeChanged();
    void updateControls();
    void showLoadMenu();

private:
    Ui::LooperWindow *ui;
    Audio::Looper *looper;
    QMap<quint8, LooperWavePanel*> wavePanels;
    Controller::MainController *mainController;

    void deleteWavePanels();


    void updateLayersVisibility(quint8 newMaxLayers);

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

    void resetAllLayersControls();

    void updateMaxLayersControls();
    void setMaxLayerComboBoxValuesAvailability(int valuesToDisable);

    void loadLoopInfo(const QString &loopsDir, const Audio::LoopInfo &info);
    void loadAudioFile(const QString &audioFilePath);

    int currentBeat;
};

Q_DECLARE_METATYPE(Audio::Looper::RecordingOption)
Q_DECLARE_METATYPE(Audio::Looper::PlayingOption)

#endif // LOOPERWINDOW_H
