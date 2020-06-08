#ifndef DYNAMICLOADLIBRARY_H
#define DYNAMICLOADLIBRARY_H
#ifdef QT_CORE_LIB
#include <QLibrary>
#include <QString>
#elif _MSC_VER
#include <Windows.h>
#else

#endif
#include <string>
#include <map>

namespace Util {

#define GET_FP(lib,FP_NAME,FP_ALIAS) \
    decltype(&::FP_NAME) FP_ALIAS = *lib.getFunction<decltype(&::FP_NAME)>(#FP_NAME);

#if !defined QT_CORE_LIB  && defined _MSC_VER
DWORD unloadDll(LPVOID Param);
#endif

#ifdef QT_CORE_LIB
    typedef QFunctionPointer MFunctionPointer;
#else
    typedef void (*MFunctionPointer)() ;
#endif

/*
 * This class is designed to conveniently access functions in a library.
*/
class DynamicLoadLibrary{

public: // Methods
    DynamicLoadLibrary();
    DynamicLoadLibrary(const std::string &filename);
    DynamicLoadLibrary(std::string &&filename);
#ifdef QT_CORE_LIB
    DynamicLoadLibrary(const QString &filename);
#endif
    virtual ~DynamicLoadLibrary();

    // Public methods
    void setDllName (const std::string& filename) {_dllName = filename;}
    void setDllName (std::string &&filename) {_dllName = std::move(filename);}
    bool load(); // Load library
    bool unload(); // Unload Library
    bool isLoaded() const { return _isLoaded; } // Loading status, including the result for resoving symbols.
    template<typename fcnPtr>
    fcnPtr getFunction(const std::string &functionName) {
        resolveSymbol(functionName);
        return reinterpret_cast<fcnPtr>(_fcnPtrStore.at(functionName));
    }

protected: // Properties
    bool _isLoaded = false; // Dll status. If library is unloaded or failed on loading, returns "false".
    std::string _dllName;

private:
    std::map<std::string,MFunctionPointer> _fcnPtrStore;
    void resolveSymbol(const std::string symbol);
#ifdef QT_CORE_LIB
    QLibrary _dllPtr;
#elif _MSC_VER
    HINSTANCE _dllPtr = nullptr;
#else
    void* _dllPtr = nullptr;
#endif

};

}

#endif // DYNAMICLOADLIBRARY_H
