#include "cellmlmodelrepositorywindow.h"
#include "cellmlmodelrepositorywidget.h"

#include "ui_cellmlmodelrepositorywindow.h"

#include <QClipboard>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QJsonParser>

namespace OpenCOR {
namespace CellMLModelRepository {

CellmlModelRepositoryWindow::CellmlModelRepositoryWindow(QWidget *pParent) :
    OrganisationWidget(pParent),
    mUi(new Ui::CellmlModelRepositoryWindow)
{
    // Set up the UI

    mUi->setupUi(this);

    // Create and add the CellML Model Repository widget

    mCellmlModelRepositoryWidget = new CellmlModelRepositoryWidget("CellmlModelRepositoryWidget",
                                                                   this);

    mUi->dockWidgetContents->layout()->addWidget(mCellmlModelRepositoryWidget);

    // We want our own context menu for the help widget (indeed, we don't want
    // the default one which has the reload menu item and not the other actions
    // that we have in our toolbar, so...)

    mCellmlModelRepositoryWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(mCellmlModelRepositoryWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(customContextMenu(const QPoint &)));

    // Retrieve the list of models in the CellML Model Repository as JSON code
    // from http://50.18.64.32/workspace/rest/contents.json

    mNetworkAccessManager = new QNetworkAccessManager(this);

    // Make sure that we get told when the download of our Internet file is
    // complete

    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(finished(QNetworkReply *)) );

    // Get the list of CellML models

    on_refreshButton_clicked();
}

CellmlModelRepositoryWindow::~CellmlModelRepositoryWindow()
{
    // Delete the UI

    delete mUi;
}

void CellmlModelRepositoryWindow::retranslateUi()
{
    // Retranslate the whole window

    mUi->retranslateUi(this);

    // Retranslate the list of models

    outputModelList(mModelList);
}

void CellmlModelRepositoryWindow::outputModelList(const QStringList &pModelList)
{
    // Output a given list of models

    mModelList = pModelList;

    QString contents = "";
    const QString leadingSpaces = "        ";

    if (mModelList.count()) {
        // We have models to list, so...

        if (mModelList.count() == 1)
            contents = tr("<strong>1</strong> CellML model was found:")+"\n";
        else
            contents = tr("<strong>%1</strong> CellML models were found:").arg(mModelList.count())+"\n";

        contents += leadingSpaces+"<ul>\n";

        foreach (const QString &model, mModelList)
            contents += leadingSpaces+"<li><a href=\""+mModelUrls.at(mModelNames.indexOf(model))+"\">"+model+"</a></li>\n";

        contents += leadingSpaces+"</ul>";
    } else if (mModelNames.empty()) {
        if (mErrorMsg.size()) {
            // Something went wrong while trying to retrieve the list of models,
            // so...

            QString errorMsg = mErrorMsg.left(1).toLower()+mErrorMsg.right(mErrorMsg.size()-1);
            QString dots = (errorMsg.at(errorMsg.size()-1) == '.')?"..":"...";

            contents = leadingSpaces+"Error: "+errorMsg+dots;
        } else {
            // The list is still being loaded, so...

            contents = leadingSpaces+tr("Please wait while the list of CellML models is being loaded...");
        }
    } else {
        // No model could be found, so...

        contents = leadingSpaces+tr("No CellML model matches your criteria");
    }

    // Output the list matching the search criteria, or a message telling the
    // user what went wrong, if anything

    mCellmlModelRepositoryWidget->output(contents);
}

void CellmlModelRepositoryWindow::on_nameValue_textChanged(const QString &text)
{
    // Generate a Web page that contains all the models which match our search
    // criteria

    outputModelList(mModelNames.filter(QRegExp(text)));
}

void CellmlModelRepositoryWindow::on_actionCopy_triggered()
{
    // Copy the current slection to the clipboard

    QApplication::clipboard()->setText(mCellmlModelRepositoryWidget->selectedText());
}

void CellmlModelRepositoryWindow::on_refreshButton_clicked()
{
    // Clear some properties

    mModelNames.clear();
    mModelUrls.clear();

    mModelList.clear();

    mErrorMsg.clear();

    // Output the message telling the user that the list is being downloaded

    outputModelList(mModelList);

    // Disable the GUI side, so that the user doesn't get confused and ask to
    // refresh over and over again while he should just be patient

    setEnabled(false);

    // Get the list of CellML models

    mNetworkAccessManager->get(QNetworkRequest(QUrl("http://50.18.64.32/workspace/rest/contents.json")));
}

void CellmlModelRepositoryWindow::finished(QNetworkReply *pNetworkReply)
{
    // Output the list of models, should we have retrieved it without any
    // problem

    if (pNetworkReply->error() == QNetworkReply::NoError) {
        // Parse the JSON code

        QJson::Parser jsonParser;
        bool parsingOk;

        QVariantMap res = jsonParser.parse (pNetworkReply->readAll(), &parsingOk).toMap();

        if (parsingOk) {
            // Retrieve the name of the keys

            QStringList keys;

            foreach (const QVariant &keyVariant, res["keys"].toList())
                keys << keyVariant.toString();

            // Retrieve the list of models itself

            foreach(const QVariant &modelVariant, res["values"].toList()) {
                QVariantList modelDetailsVariant = modelVariant.toList();

                mModelNames << modelDetailsVariant.at(0).toString();
                mModelUrls << modelDetailsVariant.at(1).toString();
            }
        }
    } else {
        // Something went wrong, so...

        mErrorMsg = pNetworkReply->errorString();
    }

    // Initialise the output using whatever search criteria is present

    on_nameValue_textChanged(mUi->nameValue->text());

    // Give the focus to the nameValue widget, just in case...

    mUi->nameValue->setFocus();

    // Re-enable the GUI side

    setEnabled(true);
}

void CellmlModelRepositoryWindow::customContextMenu(const QPoint &) const
{
    // Create a custom context menu for our CellML Models Repository widget

    QMenu menu;

    menu.addAction(mUi->actionCopy);

    menu.exec(QCursor::pos());
}

} }
