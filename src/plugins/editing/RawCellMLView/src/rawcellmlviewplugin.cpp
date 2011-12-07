//==============================================================================
// RawCellMLView plugin
//==============================================================================

#include "cellmlmodel.h"
#include "cellmlsupportplugin.h"
#include "cellmlsupportglobal.h"
#include "commonwidget.h"
#include "qscintilla.h"
#include "rawcellmlviewplugin.h"

//==============================================================================

#include <QFile>
#include <QFileInfo>
#include <QMainWindow>
#include <QTextStream>
#include <QUrl>

//==============================================================================

#include "Qsci/qscilexerxml.h"

//==============================================================================

namespace OpenCOR {
namespace RawCellMLView {

//==============================================================================

PLUGININFO_FUNC RawCellMLViewPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "A plugin to edit <a href=\"http://www.cellml.org/\">CellML</a> files using an XML editor");
    descriptions.insert("fr", "Une extension pour �diter des fichiers <a href=\"http://www.cellml.org/\">CellML</a> � l'aide d'un �diteur XML");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::Gui,
                      PluginInfo::Editing,
                      true,
                      QStringList() << "CoreCellMLEditing" << "QScintillaSupport",
                      descriptions);
}

//==============================================================================

Q_EXPORT_PLUGIN2(RawCellMLView, RawCellMLViewPlugin)

//==============================================================================

RawCellMLViewPlugin::RawCellMLViewPlugin()
{
    // Set our settings

    mGuiSettings->addView(GuiViewSettings::Editing, 0);
}

//==============================================================================

QWidget * RawCellMLViewPlugin::newViewWidget(const QString &pFileName)
{
    // Check that we are dealing with a CellML file

    if (QFileInfo(pFileName).suffix().compare(CellMLSupport::CellmlFileExtension))
        // Not the expected file extension, so...

        return GuiInterface::newViewWidget(pFileName);



    //--- TESTING --- BEGIN ---

    // Load the CellML model

    CellMLSupport::CellmlModel *cellmlModel = new CellMLSupport::CellmlModel(QUrl::fromLocalFile(pFileName));

    delete cellmlModel;

    //--- TESTING --- END ---



    // Create, set up and return a raw CellML Scintilla editor

    QFile file(pFileName);

    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
        // For some reason, the file couldn't be opened, so...

        return GuiInterface::newViewWidget(pFileName);

    // The file was properly opened, so create a Scintilla editor and associate
    // an XML (i.e. raw CellML) lexer to it

    QsciScintilla *res = new QScintillaSupport::QScintilla(QTextStream(&file).readAll(),
                                                           !(QFileInfo(pFileName).isWritable()),
                                                           new QsciLexerXML(mMainWindow),
                                                           mMainWindow);

    // We are done with the file, so close it

    file.close();

    // Our raw Scintilla editor is now ready, so...

    return res;
}

//==============================================================================

QString RawCellMLViewPlugin::viewName(const int &pViewIndex)
{
    // We have only one view, so return its name otherwise call the GuiInterface
    // implementation of viewName

    switch (pViewIndex) {
    case 0:
        return tr("Raw CellML");
    default:
        return GuiInterface::viewName(pViewIndex);
    }
}

//==============================================================================

}   // namespace RawCellMLView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
