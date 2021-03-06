//==============================================================================
// Widget
//==============================================================================

#include "widget.h"

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

Widget::Widget(QWidget *pParent) :
    QWidget(pParent),
    CommonWidget(pParent)
{
}

//==============================================================================

QSize Widget::sizeHint() const
{
    // Suggest a default size for the widget

    return defaultSize(0.15);
}

//==============================================================================

}   // namespace Core
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
