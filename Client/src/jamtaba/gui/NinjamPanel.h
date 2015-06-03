#ifndef NINJAMPANEL_H
#define NINJAMPANEL_H

#include <QWidget>
#include <QComboBox>


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
protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::NinjamPanel *ui;
};

#endif // NINJAMPANEL_H
