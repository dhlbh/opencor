//==============================================================================
// CellMLAnnotationView plugin
//==============================================================================

#include "cellmlannotationviewplugin.h"
#include "cellmlannotationviewwidget.h"
#include "cellmlfilemanager.h"

//==============================================================================

#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QSettings>

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

PLUGININFO_FUNC CellMLAnnotationViewPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "A plugin to annotate CellML files");
    descriptions.insert("fr", "Une extension pour annoter des fichiers CellML");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::Gui,
                      PluginInfo::Editing,
                      true,
                      QStringList() << "CoreCellMLEditing" << "RICORDOSupport",
                      descriptions);
}

//==============================================================================

Q_EXPORT_PLUGIN2(CellMLAnnotationView, CellMLAnnotationViewPlugin)

//==============================================================================

CellMLAnnotationViewPlugin::CellMLAnnotationViewPlugin() :
    mSizes(QList<int>())
{
    // Set our settings

    mGuiSettings->addView(GuiViewSettings::Editing, 0);
}

//==============================================================================

static const QString SettingsCellmlAnnotationWidget     = "CellmlAnnotationWidget";
static const QString SettingsCellmlAnnotationWidgetSizesCount = "CellmlAnnotationWidgetSizesCount";
static const QString SettingsCellmlAnnotationWidgetSizes      = "CellmlAnnotationWidgetSizes%1";

//==============================================================================

void CellMLAnnotationViewPlugin::loadSettings(QSettings *pSettings)
{
    // Retrieve the settings of the tree view and CellML annotation's width
    // Note #1: we would normally do this in CellmlAnnotationViewWidget, but
    //          we have one instance of it per CellML file and we want to share
    //          some information between the different instances, so we do it
    //          here...
    // Note #1: the tree view's default width is 30% of the desktop's width
    //          while of the CellML annotation is as big as it can be...
    // Note #2: because the CellML annotation's default width is much bigger
    //          than that of our tree view, the tree view's default width will
    //          effectively be less than 30% of the desktop's width, but that
    //          doesn't matter at all...

    pSettings->beginGroup(SettingsCellmlAnnotationWidget);
        int sizesCount = pSettings->value(SettingsCellmlAnnotationWidgetSizesCount, 0).toInt();

        if (!sizesCount) {
            // There are no previous sizes, so initialise things (see notes
            // above)

            mSizes << 0.3*qApp->desktop()->screenGeometry().width()
                   << qApp->desktop()->screenGeometry().width();
        } else {
            // There are previous sizes, so use them to initialise mSizes

            for (int i = 0; i < sizesCount; ++i)
                mSizes << pSettings->value(SettingsCellmlAnnotationWidgetSizes.arg(QString::number(i))).toInt();
        }
    pSettings->endGroup();
}

//==============================================================================

void CellMLAnnotationViewPlugin::saveSettings(QSettings *pSettings) const
{
    // Keep track of the tree view's and CellML annotation's width
    // Note: we must also keep track of the CellML annotation's width because
    //       when loading our settings (see above), the widget doesn't yet have
    //       a 'proper' width, so we couldn't simply assume that the Cellml
    //       annotation's initial width is this widget's width minus the tree
    //       view's initial width, so...

    pSettings->beginGroup(SettingsCellmlAnnotationWidget);
        pSettings->setValue(SettingsCellmlAnnotationWidgetSizesCount, mSizes.count());

        for (int i = 0, iMax = mSizes.count(); i < iMax; ++i)
            pSettings->setValue(SettingsCellmlAnnotationWidgetSizes.arg(QString::number(i)), mSizes.at(i));
    pSettings->endGroup();
}

//==============================================================================

QWidget * CellMLAnnotationViewPlugin::newViewWidget(const QString &pFileName)
{
    // Check that we are dealing with a CellML file and, if so, return our
    // generic raw CellML view widget

    if (!CellMLSupport::CellmlFileManager::instance()->cellmlFile(pFileName))
        // We are not dealing with a CellML file, so...

        return 0;

    // We are dealing with a CellML file, so create a new CellML annotation view
    // widget

    CellmlAnnotationViewWidget *widget = new CellmlAnnotationViewWidget(mMainWindow, pFileName);

    // Initialise our new CellML annotation view widget's sizes

    widget->updateHorizontalSplitter(mSizes);

    // Keep track of the splitter move in our new CellML annotation view widgets

    connect(widget, SIGNAL(horizontalSplitterMoved(const QList<int> &)),
            this, SLOT(horizontalSplitterMoved(const QList<int> &)));

    // Some other connections to handle splitter moves between our CellML
    // annotation view widgets

    for (int i = 0, iMax = mWidgets.count(); i < iMax; ++i) {
        // Make sur that our new CellML annotation view widget is aware of any
        // splitter move occuring in the other CellML annotation view widget

        connect(widget, SIGNAL(horizontalSplitterMoved(const QList<int> &)),
                mWidgets.at(i), SLOT(updateHorizontalSplitter(const QList<int> &)));

        // Make sur that the other CellML annotation view widgets is aware of
        // any splitter move occuring in our new CellML annotation view widget

        connect(mWidgets.at(i), SIGNAL(horizontalSplitterMoved(const QList<int> &)),
                widget, SLOT(updateHorizontalSplitter(const QList<int> &)));
    }

    // Keep track of our new CellML annotation view widget

    mWidgets.append(widget);

    // We are all done, so return our new CellML annotation view widget

    return widget;
}

//==============================================================================

QString CellMLAnnotationViewPlugin::viewName(const int &pViewIndex)
{
    // We have only one view, so return its name otherwise call the GuiInterface
    // implementation of viewName

    switch (pViewIndex) {
    case 0:
        return tr("CellML Annotation");
    default:
        return GuiInterface::viewName(pViewIndex);
    }
}

//==============================================================================

void CellMLAnnotationViewPlugin::horizontalSplitterMoved(const QList<int> &pSizes)
{
    // The horizontal splitter of one of our CellML annotation view widgets has
    // been moved, so update

    mSizes = pSizes;
}

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
