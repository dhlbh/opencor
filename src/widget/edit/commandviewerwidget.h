#ifndef COMMANDVIEWERWIDGET_H
#define COMMANDVIEWERWIDGET_H

#include "commonwidget.h"

#include <QWidget>

class CommandViewerWidget : public QWidget, public CommonWidget
{
    Q_OBJECT

public:
    virtual void retranslateUi();

    virtual void defaultSettings();

    virtual void loadSettings(const QSettings &, const QString &);
    virtual void saveSettings(QSettings &, const QString &);

protected:
    virtual QSize sizeHint() const;
};

#endif
