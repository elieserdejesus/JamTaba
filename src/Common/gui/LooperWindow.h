#ifndef LOOPERWINDOW_H
#define LOOPERWINDOW_H

#include <QDialog>
#include <QCheckBox>
#include <QMap>
#include <QLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

#include "looper/Looper.h"
#include "looper/LooperPersistence.h"
#include "looper/LooperLayer.h"
#include "widgets/BlinkableButton.h"
#include "widgets/Slider.h"
#include "widgets/LooperWavePanel.h"

namespace controller {
class NinjamController;
class MainController;
}

namespace Ui {
class LooperWindow;
}

using audio::Looper;
using audio::LooperLayer;
using audio::LooperState;
using audio::LoopSaver;
using audio::LoopLayerInfo;
using audio::AudioPeak;
using controller::MainController;

class LooperWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LooperWindow(QWidget *parent, MainController *mainController);
    ~LooperWindow();

    void setLooper(Looper *looper);
    void detachCurrentLooper();
    void updateDrawings();
    void setTintColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void changeEvent(QEvent *ev);

private slots:
    void updateBeatsPerInterval();
    void updateCurrentBeat(uint currentIntervalBeat);
    void handleNewMaxLayers(quint8 newMaxLayers);
    void handleModeChanged();
    void updateControls();
    void showLoadMenu();
    void showResetMenu();
    void loadAudioFilesIntoLayer(const QStringList &audioFilePaths, qint8 firstLayerIndex);
    void loadAudioFiles(const QStringList &audioFilePaths);
    void handleLayerMuteStateChanged(quint8 layer, quint8 state);

    void resetLayersControls();
    void resetAll();

    void showSaveDialogs();

private:
    Ui::LooperWindow *ui;
    Looper *looper;

    class LayerControlsLayout : public QHBoxLayout
    {
    public:
        LayerControlsLayout(Looper *looper, quint8 layerIndex);
        void setMuteButtonVisibility(bool show);
        void enableMuteButton(bool enabled);

        PanSlider *panSlider;
        AudioSlider *gainSlider;
        QLabel *labelPanL;
        QLabel *labelPanR;
        BlinkableButton *muteButton;
    };

    struct LayerView
    {
        LayerView(LooperWavePanel *wavePanel, LayerControlsLayout *controlsLayout)
            : wavePanel(wavePanel),
              controlsLayout(controlsLayout)
        {
            //
        }

        LayerView()
            : wavePanel(nullptr),
              controlsLayout(nullptr)
        {

        }

        LooperWavePanel *wavePanel;
        LayerControlsLayout *controlsLayout;
    };

    QMap<quint8, LayerView> layerViews;

    controller::MainController *mainController;

    void deleteWavePanels();


    void updateLayersVisibility(quint8 newMaxLayers);

    void addSamplesPeak(float peak, uint samplesCount, quint8 layerIndex);

    void initializeControls();

    void createRecordingOptionsCheckBoxes();
    void createPlayingOptionsCheckBoxes();

    QMap<Looper::RecordingOption, QCheckBox *> recordingCheckBoxes;
    QMap<Looper::PlayingOption, QCheckBox *> playingCheckBoxes;

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
                if (looper) {
                    looper->setOption(option, checked);
                }
            });
        }
    }


    void clearLayout(QLayout *layout);

    static QString getOptionName(Looper::RecordingOption option);
    static QString getOptionName(Looper::PlayingOption option);

    static QString getOptionToolTip(Looper::RecordingOption option);
    static QString getOptionToolTip(Looper::PlayingOption option);

    static QList<Looper::RecordingOption> getAllRecordingOptions();
    static QList<Looper::PlayingOption> getAllPlayingOptions();

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
                checkBox->setVisible(optionIsSupported);
            }
            else{
                qCritical() << "Checkbox is null!";
            }
        }
    }

    template<class OptionType>
    void translateOptions(QLayout *layout)
    {
        for (int i = 0; i < layout->count(); ++i) {
            QCheckBox *checkBox = qobject_cast<QCheckBox *>(layout->itemAt(i)->widget());
            if (checkBox) {
                OptionType option = qvariant_cast<OptionType>(checkBox->property("option"));
                checkBox->setText(getOptionName(option));
                checkBox->setToolTip(getOptionToolTip(option));
            }
        }
    }

    void updateMaxLayersControls();
    void setMaxLayerComboBoxValuesAvailability(int valuesToDisable);

    void loadLoopInfo(const QString &loopDir, const audio::LoopInfo &info);

    void updateLayersControls();
    void updateModeComboBox();

    void updateButtons();

    void connectLooperSignals();
    void disconnectLooperSignals();

    int currentBeat;

    QColor tintColor;
};

Q_DECLARE_METATYPE(audio::Looper::RecordingOption)
Q_DECLARE_METATYPE(audio::Looper::PlayingOption)

#endif // LOOPERWINDOW_H
