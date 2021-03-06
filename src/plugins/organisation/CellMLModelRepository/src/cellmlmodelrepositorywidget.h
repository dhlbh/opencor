//==============================================================================
// CellML Model Repository widget
//==============================================================================

#ifndef CELLMLMODELREPOSITORYWIDGET_H
#define CELLMLMODELREPOSITORYWIDGET_H

//==============================================================================

#include "commonwidget.h"

//==============================================================================

#include <QWebView>

//==============================================================================

namespace OpenCOR {
namespace CellMLModelRepository {

//==============================================================================

class CellmlModelRepositoryWidget : public QWebView, public Core::CommonWidget
{
    Q_OBJECT

public:
    explicit CellmlModelRepositoryWidget(QWidget *pParent);

    void output(const QString &pOutput);

protected:
    virtual QSize sizeHint() const;

    virtual void changeEvent(QEvent *pEvent);
    virtual void paintEvent(QPaintEvent *pEvent);

private:
    QString mOutputTemplate;

private Q_SLOTS:
    void openLink(const QUrl &pUrl);
};

//==============================================================================

}   // namespace CellMLModelRepository
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
