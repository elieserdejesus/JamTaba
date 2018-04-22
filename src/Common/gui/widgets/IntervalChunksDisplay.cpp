#include "IntervalChunksDisplay.h"
#include <QDebug>

IntervalChunksDisplay::IntervalChunksDisplay(QWidget *parent) :
    QProgressBar(parent),
    downloadedChunks(0),
    totalChunks(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    setToolTip(tr("Downloaded intervals"));
    setAccessibleDescription(toolTip());
    setValue(totalChunks);
    setTextVisible(false);
}

void IntervalChunksDisplay::setEstimatedTotalChunks(int estimatedChunks)
{
    totalChunks = estimatedChunks;
    setMaximum(totalChunks);
}

void IntervalChunksDisplay::incrementDownloadedChunks()
{
    downloadedChunks++;
    if(downloadedChunks > totalChunks)
        setMaximum(downloadedChunks + 1);
    setValue(downloadedChunks);
}

void IntervalChunksDisplay::finish()
{
    totalChunks = downloadedChunks;
    downloadedChunks = 0;
    setMaximum(totalChunks);
    setValue(downloadedChunks);

    setVisible(false);
}

void IntervalChunksDisplay::reset()
{
    downloadedChunks = 0;
    setValue(0);
}
