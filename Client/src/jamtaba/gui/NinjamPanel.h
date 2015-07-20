#ifndef NINJAMPANEL_H
#define NINJAMPANEL_H

#include <QWidget>
#include <QStringList>
class QComboBox;
class QSlider;
class QPushButton;
class QObject;
class QEvent;

//#include <QComboBox>
//#include <QStringListModel>

namespace Ui {
class NinjamPanel;
}

class NinjamPanel : public QWidget
{
    Q_OBJECT


public:
    explicit NinjamPanel(QWidget *parent = 0);
    ~NinjamPanel();
    QComboBox* getBpiCombo() const;
    QComboBox* getBpmCombo() const;
    QComboBox* getAccentsCombo() const;
    QSlider* getGainSlider() const;
    QSlider* getPanSlider() const;
    QPushButton* getMuteButton() const;
    QPushButton* getSoloButton() const;

    void setBpi(int bpi);
    void setBpm(int bpm);
    void setCurrentBeat(int currentBeat);

    void setMetronomePeaks(float left, float right);
protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *source, QEvent *ev);
    Ui::NinjamPanel *ui;
private:

    void buildAccentsdModel(int bpi);
    QStringList getDividers(int bpi);
    static bool compareBpis(const QString& s1, const QString& s2);
private slots:
    void comboAccentsChanged(int index);
};

#endif // NINJAMPANEL_H
