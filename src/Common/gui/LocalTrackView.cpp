#include "LocalTrackView.h"
#include "MainController.h"
#include "audio/core/LocalInputNode.h"
#include "GuiUtils.h"
#include "widgets/BoostSpinBox.h"
#include "IconFactory.h"

#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QStyle>
#include <QPainter>
#include <QIcon>
#include <QFontMetrics>

class LocalTrackView::LooperIconFactory
{
public:

    explicit LooperIconFactory(const QString &originalIconPath)
        : originalIconPath(originalIconPath)
    {
        //
    }

    QIcon createRecordingIcon() const
    {
        // create recording icon
        QList<QSize> iconSizes = originalIcon.availableSizes();
        if (!iconSizes.isEmpty()) {
            QPixmap recPixmap = originalIcon.pixmap(iconSizes.first());

            QPainter painter(&recPixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(QColor(255, 0, 0, 120));
            painter.setPen(Qt::NoPen);
            QRectF rect(recPixmap.rect());
            painter.drawEllipse(rect.marginsAdded(QMarginsF(-2, -5, -2, -2)));
            //painter.drawEllipse(recPixmap.width() - radius, recPixmap.height() - radius, radius, radius);

            return QIcon(recPixmap);
        }

        qCritical() << "icon available sizes is empty!";
        return originalIcon;
    }

    QIcon createCurrentLooperLayerIcon(quint8 currentLayer, const QFontMetricsF &fontMetrics)
    {
        QIcon layerIcon = layersIcons[currentLayer];
        if (layerIcon.isNull()) {
            QList<QSize> sizes = originalIcon.availableSizes();
            if (!sizes.isEmpty()) {
                QPixmap pixmap = originalIcon.pixmap(sizes.first());
                QPainter painter(&pixmap);
                painter.setRenderHint(QPainter::TextAntialiasing);
                painter.setRenderHint(QPainter::Antialiasing);

                QString text(QString::number(currentLayer + 1));
                qreal rectWidth = fontMetrics.width(text) * 2;

                QRectF textRect(pixmap.width() - rectWidth, pixmap.height() - fontMetrics.height(), rectWidth, fontMetrics.height());

                painter.setBrush(QColor(255, 255, 255, 150));
                painter.setPen(Qt::black);
                painter.drawEllipse(textRect);
                painter.drawText(textRect, text, QTextOption(Qt::AlignCenter));

                layerIcon = QIcon(pixmap);
                layersIcons.insert(currentLayer, layerIcon);
            }
            else {
                qCritical() << "sizes is empty!";
            }
        }

        return layerIcon;
    }

    QIcon getIcon(audio::Looper *looper, const QFontMetricsF &fontMetrics)
    {
        if (originalIcon.isNull()) {
            this->originalIcon = QIcon(originalIconPath);
            this->recordingIcon = createRecordingIcon();
        }

        if (looper) {
            if (looper->isRecording() || looper->isWaitingToRecord()) {
                return recordingIcon;
            }
            else if (looper->isPlaying()) {
                quint8 currentLayer = looper->getCurrentLayerIndex();
                return createCurrentLooperLayerIcon(currentLayer, fontMetrics);
            }
        }

        return originalIcon;
    }

private:
    QString originalIconPath;
    QIcon originalIcon;
    QIcon recordingIcon;
    QMap<quint8, QIcon> layersIcons;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++

LocalTrackView::LooperIconFactory LocalTrackView::looperIconFactory(":/images/loop.png");

LocalTrackView::LocalTrackView(controller::MainController *mainController, int channelIndex) :
    BaseTrackView(mainController, channelIndex),
    inputNode(nullptr),
    buttonStereoInversion(createStereoInversionButton()),
    buttonLooper(createLooperButton()),
    peakMetersOnly(false)
{
    Q_ASSERT(mainController);

    // insert a input node in controller
    inputNode = new audio::LocalInputNode(mainController, channelIndex);
    trackID = mainController->addInputTrackNode(this->inputNode);
    bindThisViewWithTrackNodeSignals();// now is secure bind this LocalTrackView with the respective TrackNode model

    setInitialValues(1.0f, BaseTrackView::Boost::ZERO, 0.0f, false, false);

    setActivatedStatus(false);

    secondaryChildsLayout->addWidget(buttonLooper, 0, Qt::AlignCenter);
    secondaryChildsLayout->addWidget(buttonStereoInversion, 0, Qt::AlignCenter);

    connect(inputNode->getLooper(), &audio::Looper::stateChanged, this, &LocalTrackView::updateLooperButtonIcon);
    connect(inputNode->getLooper(), &audio::Looper::currentLayerChanged, this, &LocalTrackView::updateLooperButtonIcon);
}

void LocalTrackView::updateLooperButtonIcon()
{
    // get a new icon based in looper state
    QIcon newIcon = looperIconFactory.getIcon(inputNode->getLooper(), buttonLooper->fontMetrics());
    buttonLooper->setIcon(newIcon);
}

void LocalTrackView::bindThisViewWithTrackNodeSignals()
{
    BaseTrackView::bindThisViewWithTrackNodeSignals();

    connect(inputNode, &audio::LocalInputNode::stereoInversionChanged, this, &LocalTrackView::setStereoInversion);
}

void LocalTrackView::setInitialValues(float initialGain, BaseTrackView::Boost boostValue,
                                      float initialPan, bool muted, bool stereoInverted)
{
    inputNode->setGain(initialGain);
    inputNode->setPan(initialPan);
    initializeBoostSpinBox(boostValue);
    if (muted)
        inputNode->setMute(muted);

    setStereoInversion(stereoInverted);
}

void LocalTrackView::detachMainController()
{
    this->mainController = nullptr;
}

void LocalTrackView::closeAllPlugins()
{
    inputNode->closeProcessorsWindows(); // close vst editors
}

void LocalTrackView::mute(bool b)
{
    getInputNode()->setMute(b); // audio only
    muteButton->setChecked(b); // gui only
}

void LocalTrackView::solo(bool b)
{
    getInputNode()->setSolo(b); // audio only
    soloButton->setChecked(b); // gui only
}

void LocalTrackView::initializeBoostSpinBox(Boost boostValue)
{
    if (boostValue == Boost::PLUS)
        boostSpinBox->setToMax();
    else if (boostValue == Boost::MINUS)
        boostSpinBox->setToMin();
    else
        boostSpinBox->setToOff(); // 0 dB - OFF
}

QSize LocalTrackView::sizeHint() const
{
    if (peakMetersOnly) {
        return QSize(16, height());
    }
    return BaseTrackView::sizeHint();
}

void LocalTrackView::setupMetersLayout()
{
    metersLayout->addWidget(peakMeter);
}

void LocalTrackView::setPeakMetersOnlyMode(bool peakMetersOnly)
{
    if (this->peakMetersOnly != peakMetersOnly) {
        this->peakMetersOnly = peakMetersOnly;

        gui::setLayoutItemsVisibility(secondaryChildsLayout, !this->peakMetersOnly);
        gui::setLayoutItemsVisibility(primaryChildsLayout, !this->peakMetersOnly);

        if (peakMetersOnly) { // add the peak meters directly in main layout, so these meters are horizontally centered
            mainLayout->addWidget(peakMeter, 0, 0);
        }
        else { // put the meter in the original layout
            setupMetersLayout();
        }

        const int spacing = peakMetersOnly ? 0 : 3;

        mainLayout->setHorizontalSpacing(spacing);

        peakMeter->setVisible(true); // peak meter are always visible

        peakMeter->setPaintingDbMarkers(!peakMetersOnly);

        QMargins margins = layout()->contentsMargins();
        margins.setLeft(spacing);
        margins.setRight(spacing);
        layout()->setContentsMargins(margins);

        soloButton->setVisible(!peakMetersOnly);
        muteButton->setVisible(!peakMetersOnly);
        Qt::Alignment alignment = peakMetersOnly ? Qt::AlignRight : Qt::AlignHCenter;
        levelSlider->parentWidget()->layout()->setAlignment(levelSlider, alignment);

        updateGeometry();

        setProperty("peakMetersOnly", QVariant(peakMetersOnly));
        style()->unpolish(this);
        style()->polish(this);
    }
}

void LocalTrackView::togglePeakMetersOnlyMode()
{
    setPeakMetersOnlyMode(!peakMetersOnly);
}

void LocalTrackView::setActivatedStatus(bool unlighted)
{
    BaseTrackView::setActivatedStatus(unlighted);
    update();
}

audio::LocalInputNode *LocalTrackView::getInputNode() const
{
    return inputNode;
}

void LocalTrackView::reset()
{
    mainController->resetTrack(getTrackID());
}

LocalTrackView::~LocalTrackView()
{
    if (mainController)
        mainController->removeInputTrackNode(getTrackID());
}

void LocalTrackView::setTintColor(const QColor &color)
{
    BaseTrackView::setTintColor(color);

    buttonLooper->setIcon(IconFactory::createLooperButtonIcon(color));

    buttonStereoInversion->setIcon(IconFactory::createStereoInversionIcon(color));
}

QPushButton *LocalTrackView::createLooperButton()
{
    QPushButton *button = new QPushButton(IconFactory::createLooperButtonIcon(getTintColor()), "");
    button->setObjectName(QStringLiteral("buttonLooper"));
    button->setEnabled(false); // disabled by default

    connect(button, &QPushButton::clicked, [=]{
        emit openLooperEditor(this->trackID);
    });

    return button;
}

void LocalTrackView::enableLopperButton(bool enabled)
{
    buttonLooper->setEnabled(enabled && !getInputNode()->isRoutingMidiInput());
}

QPushButton *LocalTrackView::createStereoInversionButton()
{
    QPushButton *button = new QPushButton();
    button->setObjectName(QStringLiteral("buttonStereoInversion"));
    button->setCheckable(true);
    connect(button, &QPushButton::clicked, this, &LocalTrackView::setStereoInversion);
    return button;
}

void LocalTrackView::translateUI()
{
    BaseTrackView::translateUI();

    buttonStereoInversion->setToolTip(tr("Invert stereo"));
    buttonLooper->setToolTip(tr("Looper (Available when jamming)"));

}

void LocalTrackView::setStereoInversion(bool stereoInverted)
{
    mainController->setTrackStereoInversion(getInputIndex(), stereoInverted);
    buttonStereoInversion->setChecked(stereoInverted);
}

void LocalTrackView::updateStyleSheet()
{
    BaseTrackView::updateStyleSheet();

    buttonLooper->setIcon(IconFactory::createLooperButtonIcon(getTintColor()));

    style()->unpolish(buttonStereoInversion); // this is necessary to change the stereo inversion button colors when the transmit button is clicled
    style()->polish(buttonStereoInversion);

    style()->unpolish(buttonLooper);
    style()->polish(buttonLooper);
}
