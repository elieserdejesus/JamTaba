#ifndef NINJAMPANEL_H
#define NINJAMPANEL_H

#include <QWidget>
#include <QComboBox>
#include <QStringListModel>

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

    void setBpi(int bpi);
    void setBpm(int bpm);
    void setCurrentBeat(int currentBeat);
protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::NinjamPanel *ui;
    void buildAccentsdModel(int bpi);
    QStringList getDividers(int bpi);
    static bool compareBpis(const QString& s1, const QString& s2);
private slots:
    void comboAccentsChanged(int index);
};

#endif // NINJAMPANEL_H
