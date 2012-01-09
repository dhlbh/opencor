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
                                 const int &pLine = 0, const int &pColumn = 0);

    QString message() const;
    QString formattedMessage() const;
    int line() const;
    int column() const;

private:
    QString mMessage;
    int mLine;
    int mColumn;
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

    llvm::Function * jitCode() const;

    Type type() const;
    void setType(const Type &pType);

    QString name() const;
    void setName(const QString &pName);

    QStringList parameterNames() const;
    bool addParameterName(const QString &pParameterName);

private:
    llvm::Function * mJitCode;

    Type mType;
    QString mName;
    QStringList mParameterNames;
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

    void addIssue(const CompilerScannerToken &pToken, const QString &pExpected);

    bool parseFunction(CompilerScanner &pScanner,
                       CompilerEngineFunction &pFunction);
    bool parseParameters(CompilerScanner &pScanner,
                         CompilerEngineFunction &pFunction);
    bool parseEquations(CompilerScanner &pScanner,
                        CompilerEngineFunction &pFunction);
    bool parseDoubleValue(CompilerScanner &pScanner,
                          CompilerEngineFunction &pFunction);
    bool parseEquationRhs(CompilerScanner &pScanner,
                          CompilerEngineFunction &pFunction);
    bool parseReturn(CompilerScanner &pScanner,
                     CompilerEngineFunction &pFunction);

    void compileFunction(CompilerEngineFunction &pFunction);
};

//==============================================================================

}   // namespace Compiler
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
