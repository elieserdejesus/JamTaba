#include "BusyDialog.h"
#include "ui_BusyDialog.h"
#include <QMovie>

BusyDialog::BusyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BusyDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);

    QMovie *movie = new QMovie(":/loading.gif");
    movie->setParent(this);
    movie->setScaledSize(QSize(64, 64));
    ui->labelGif->setMovie(movie);
}

void BusyDialog::show(const QString &message)
{
    ui->labelText->setText(message);
    QDialog::show();
}

void BusyDialog::show()
{
    show("");
}

void BusyDialog::showEvent(QShowEvent *e)
{
    Q_UNUSED(e);
    ui->labelGif->movie()->start();
}

void BusyDialog::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e);
    ui->labelGif->movie()->stop();
}

QRegion BusyDialog::roundedRect(const QRect &rect, int round)
{
    QRegion region;

    // middle and borders
    region += rect.adjusted(round, 0, -round, 0);
    region += rect.adjusted(0, round, 0, -round);

    // top left
    QRect corner(rect.topLeft(), QSize(round*2, round*2));
    region += QRegion(corner, QRegion::Ellipse);

    // top right
    corner.moveTopRight(rect.topRight());
    region += QRegion(corner, QRegion::Ellipse);

    // bottom left
    corner.moveBottomLeft(rect.bottomLeft());
    region += QRegion(corner, QRegion::Ellipse);

    // bottom right
    corner.moveBottomRight(rect.bottomRight());
    region += QRegion(corner, QRegion::Ellipse);

    return region;
}

BusyDialog::~BusyDialog()
{
    delete ui;
}
