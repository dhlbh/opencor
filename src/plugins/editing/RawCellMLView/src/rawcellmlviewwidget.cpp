//==============================================================================
// Raw CellML view widget
//==============================================================================

#include "qscintilla.h"
#include "rawcellmlviewwidget.h"
#include "viewerwidget.h"

//==============================================================================

#include "ui_rawcellmlviewwidget.h"

//==============================================================================

#include <QDesktopWidget>
#include <QFileInfo>
#include <QSplitter>
#include <QTextStream>

//==============================================================================

#include "Qsci/qscilexerxml.h"

//==============================================================================

namespace OpenCOR {
namespace RawCellMLView {

//==============================================================================

RawCellmlViewWidget::RawCellmlViewWidget(QWidget *pParent) :
    Widget(pParent),
    mUi(new Ui::RawCellmlViewWidget),
    mEditors(QMap<QString, QScintillaSupport::QScintilla *>()),
    mEditor(0)
{
    // Set up the UI

    mUi->setupUi(this);

    // Create our vertical splitter

    mVerticalSplitter = new QSplitter(Qt::Vertical, this);

    // Create a viewer

    mViewer = new Viewer::ViewerWidget(pParent);

    // Populate our vertical splitter and add it to our raw CellML view widget

    mVerticalSplitter->addWidget(mViewer);

    mUi->verticalLayout->addWidget(mVerticalSplitter);
}

//==============================================================================

void RawCellmlViewWidget::initialize(const QString &pFileName)
{
    // Retrieve the size of our viewer and current editor, and hide the editor

    bool needInitialSizes = !mEditor;

    int viewerHeight = 0;
    int editorHeight = 0;

    if (mEditor) {
        // An editor is currently available, so retrieve the size of both our
        // viewer and the current editor

        viewerHeight = mViewer->height();
        editorHeight = mEditor->height();

        // Hide the current editor

        mEditor->hide();
    }

    // Retrieve the editor associated with the file name, if any

    mEditor = mEditors.value(pFileName);

    if (!mEditor) {
        // No editor exists for the file name, so create and set up a Scintilla
        // editor with an XML lexer associated to it

        QFile file(pFileName);
        QString fileContents = QString();
        bool fileIsWritable = false;

        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            // We could open the file, so retrieve its contents and whether it
            // can be written to

            fileContents = QTextStream(&file).readAll();
            fileIsWritable = !(QFileInfo(pFileName).isWritable());

            // We are done with the file, so close it

            file.close();
        }

        mEditor = new QScintillaSupport::QScintilla(fileContents, fileIsWritable,
                                                    new QsciLexerXML(parent()),
                                                    qobject_cast<QWidget *>(parent()));

        // Keep track of the editor and add it to our vertical splitter

        mEditors.insert(pFileName, mEditor);

        mVerticalSplitter->addWidget(mEditor);
    }

    // Make sure that the 'new' editor is visible

    mEditor->show();

    // Adjust our vertical splitter's sizes

    if (needInitialSizes) {
        // We need to initialise our vertical splitter's sizes and we do so by
        // using as much space as possible for the editor by asking the viewer
        // to take only 10% of the desktop's height and the editor to take
        // whatever space it can

        mVerticalSplitter->setSizes(QList<int>() << 0.1*qApp->desktop()->screenGeometry().height()
                                                 << qApp->desktop()->screenGeometry().height());
    } else {
        // Our vertical splitter's sizes have already been initialised, so set
        // its sizes so that our 'new' editor gets its size set to that of the
        // 'old' editor

        QList<int> newSizes = QList<int>() << viewerHeight;

        for (int i = 1, iMax = mVerticalSplitter->count(); i < iMax; ++i)
            if (dynamic_cast<QScintillaSupport::QScintilla *>(mVerticalSplitter->widget(i)) == mEditor)
                // This the editor we are after, so...

                newSizes << editorHeight;
            else
                // Not the editor we are after, so...

                newSizes << 0;

        mVerticalSplitter->setSizes(newSizes);
    }
}

//==============================================================================

}   // namespace RawCellMLView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
