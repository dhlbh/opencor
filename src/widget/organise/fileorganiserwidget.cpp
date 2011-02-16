#include "fileorganiserwidget.h"

FileOrganiserWidget::FileOrganiserWidget(QWidget *pParent) :
    QWidget(pParent),
    CommonWidget(pParent)
{
}

void FileOrganiserWidget::retranslateUi()
{
    // Nothing to do for now...
}

void FileOrganiserWidget::defaultSettings()
{
    // Nothing to do for now...
}

void FileOrganiserWidget::loadSettings(const QSettings &, const QString &)
{
    // Nothing to do for now...
}

void FileOrganiserWidget::saveSettings(QSettings &, const QString &)
{
    // Nothing to do for now...
}

QSize FileOrganiserWidget::sizeHint() const
{
    // Suggest a default size for the file organiser widget
    // Note: this is critical if we want a docked widget, with a file organiser
    //       widget on it, to have a decent size when docked to the main window

    return defaultSize(0.15);
}

void FileOrganiserWidget::paintEvent(QPaintEvent *pEvent)
{
    QWidget::paintEvent(pEvent);

    // Draw a border in case we are docked

    drawBorderIfDocked();
}
