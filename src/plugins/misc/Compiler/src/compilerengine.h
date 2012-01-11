//==============================================================================
// Compiler engine class
//==============================================================================

#ifndef COMPILERENGINE_H
#define COMPILERENGINE_H

//==============================================================================

#include "compilerglobal.h"
#include "compilerscanner.h"

//==============================================================================

#include <QStringList>

//==============================================================================

#include "llvm/Function.h"

//==============================================================================

namespace OpenCOR {
namespace Compiler {

//==============================================================================

class COMPILER_EXPORT CompilerEngineIssue
{
public:
    explicit CompilerEngineIssue(const QString &pMessage,
                                 const int &pLine = 0, const int &pColumn = 0,
                                 const QString &pExtraInformation = QString());

    QString message() const;
    QString formattedMessage() const;
    int line() const;
    int column() const;
    QString extraInformation() const;

private:
    QString mMessage;
    int mLine;
    int mColumn;
    QString mExtraInformation;
};

//==============================================================================

class CompilerEngineFunction
{
public:
    enum Type {
        Void,
        Double
    };

    explicit CompilerEngineFunction();

    llvm::Function * irCode() const;
    void setIrCode(llvm::Function *pIrCode);

    Type type() const;
    void setType(const Type &pType);

    QString name() const;
    void setName(const QString &pName);

    QStringList parameters() const;
    bool addParameter(const QString &pParameter);

    QString returnValue() const;
    void setReturnValue(const QString &pReturnValue);

private:
    llvm::Function * mIrCode;

    Type mType;
    QString mName;
    QStringList mParameters;
    QString mReturnValue;
};

//==============================================================================

class COMPILER_EXPORT CompilerEngine : public QObject
{
    Q_OBJECT

public:
    explicit CompilerEngine();
    ~CompilerEngine();

    llvm::Module * module();

    QList<CompilerEngineIssue> issues();

    llvm::Function * addFunction(const QString &pFunction);

private:
    llvm::Module *mModule;

    QList<CompilerEngineIssue> mIssues;

    void addIssue(const CompilerScannerToken &pToken, const QString &pMessage,
                  const bool &pExpectedMessage = true,
                  const QString &pExtraInformation = QString());

    bool parseFunction(CompilerScanner &pScanner,
                       CompilerEngineFunction &pFunction);
    bool parseParameter(CompilerScanner &pScanner,
                        CompilerEngineFunction &pFunction,
                        const bool &pNeeded = true);
    bool parseParameters(CompilerScanner &pScanner,
                         CompilerEngineFunction &pFunction);
    bool parseEquations(CompilerScanner &pScanner,
                        CompilerEngineFunction &pFunction);
    bool parseEquationRhs(CompilerScanner &pScanner,
                          CompilerEngineFunction &pFunction);
    bool parseReturn(CompilerScanner &pScanner,
                     CompilerEngineFunction &pFunction);

    bool compileFunction(CompilerEngineFunction &pFunction);
};

//==============================================================================

}   // namespace Compiler
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
