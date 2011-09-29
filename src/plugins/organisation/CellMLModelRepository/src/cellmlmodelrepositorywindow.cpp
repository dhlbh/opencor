#include "cellmlmodelrepositorywindow.h"
#include "cellmlmodelrepositorywidget.h"

#include "ui_cellmlmodelrepositorywindow.h"

#include <QJsonParser>

namespace OpenCOR {
namespace CellMLModelRepository {

CellmlModelRepositoryWindow::CellmlModelRepositoryWindow(QWidget *pParent) :
    DockWidget(pParent),
    mUi(new Ui::CellmlModelRepositoryWindow),
    mErrorMsg(QString())
{
    // Set up the UI

    mUi->setupUi(this);

    // Create and add the CellML Model Repository widget

    mCellmlModelRepositoryWidget = new CellmlModelRepositoryWidget("CellmlModelRepositoryWidget",
                                                                   this);

    mUi->dockWidgetContents->layout()->addWidget(mCellmlModelRepositoryWidget);

    // Retrieve the list of models in the CellML Model Repository as JSON code
    // from http://50.18.64.32/workspace/rest/contents.json

    QNetworkAccessManager * networkAccessManager = new QNetworkAccessManager(this);

    // Make sure that we get told when the download of our Internet file is
    // complete

    connect(networkAccessManager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(finished(QNetworkReply *)) );

    // Get our Internet file

    networkAccessManager->get(QNetworkRequest(QUrl("http://50.18.64.32/workspace/rest/contents.json")));
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

    if (mModelList.count()) {
        // We have models to list, so...

        if (mModelList.count() == 1)
            contents = tr("<strong>1</strong> CellML model was found:")+"\n";
        else
            contents = tr("<strong>%1</strong> CellML models were found:").arg(mModelList.count())+"\n";

        contents += "        <ul>\n";

        foreach (const QString &model, mModelList)
            contents += "            <li><a href=\""+mModelUrls.at(mModelNames.indexOf(model))+"\">"+model+"</a></li>\n";

        contents += "        </ul>";
    } else if (mModelNames.empty()) {
        if (mErrorMsg.size())
            // Something went wrong while trying to retrieve the list of models,
            // so...

            contents = mErrorMsg+"...";
        else
            // The list is still being loaded, so...

            contents = "        "+tr("Please wait while the list of CellML models is being loaded...");
    } else {
        // No model could be found, so...

        contents = "        "+tr("No CellML model matches your criteria");
    }

    mCellmlModelRepositoryWidget->output(contents);
}

void CellmlModelRepositoryWindow::on_nameValue_textChanged(const QString &text)
{
    // Generate a Web page that contains all the models which match our search
    // criteria

    outputModelList(mModelNames.filter(text));
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
                keys.append(keyVariant.toString());

            // Retrieve the list of models itself

            foreach(const QVariant &modelVariant, res["values"].toList()) {
                QVariantList modelDetailsVariant = modelVariant.toList();

                mModelNames.append(modelDetailsVariant.at(0).toString());
                mModelUrls.append(modelDetailsVariant.at(1).toString());
            }
        }
    } else {
        // Something went wrong, so...

        mErrorMsg = pNetworkReply->errorString();
    }

    // Initialise the output with all of the models

    outputModelList(mModelNames);
}

} }
