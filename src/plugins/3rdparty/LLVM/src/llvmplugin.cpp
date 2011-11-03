//==============================================================================
// LLVM plugin
//==============================================================================

#include "llvmplugin.h"

//==============================================================================

namespace OpenCOR {
namespace LLVM {

//==============================================================================

PLUGININFO_FUNC LLVMPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "A plugin to use the <a href=\"http://en.wikipedia.org/wiki/Just-in-time_compilation\">just-in-time compilation</a> side of <a href=\"http://www.llvm.org/\">LLVM</a>");
    descriptions.insert("fr", "Une extension pour utiliser le c�t� <a href=\"http://fr.wikipedia.org/wiki/Compilation_�_la_vol�e\">compilation � la vol�e</a> de <a href=\"http://www.llvm.org/\">LLVM</a>");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::General,
                      PluginInfo::ThirdParty,
                      false,
                      QStringList(),
                      descriptions);
}

//==============================================================================

Q_EXPORT_PLUGIN2(LLVM, LLVMPlugin)

//==============================================================================

}   // namespace LLVM
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
