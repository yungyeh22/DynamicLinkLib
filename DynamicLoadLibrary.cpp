#include "DynamicLoadLibrary.h"

namespace Util {

#if !defined QT_CORE_LIB  && defined _MSC_VER
DWORD unloadDll(LPVOID Param) {
    //	Param should be the address of the dllhandle
    HINSTANCE *dllhandle = (HINSTANCE*)Param;
    //	Finally, unload the DLL
    if (FreeLibrary(*dllhandle)) {
        *dllhandle = nullptr;
    }
    return 0;
}
#endif

DynamicLoadLibrary::DynamicLoadLibrary() {

}

#ifdef QT_CORE_LIB
DynamicLoadLibrary::DynamicLoadLibrary(const QString &filename) : _dllName(filename.toStdString()) {

}
#endif

DynamicLoadLibrary::DynamicLoadLibrary(const std::string &filename) : _dllName(filename) {

}

DynamicLoadLibrary::DynamicLoadLibrary(std::string &&filename) : _dllName(std::move(filename)) {

}

DynamicLoadLibrary::~DynamicLoadLibrary() {
    unload();
}

bool DynamicLoadLibrary::load() {
    bool loadSuccess = false;
    if (!_dllName.empty()) {
        if (_isLoaded) {
            loadSuccess = true;
        }
        else {
#ifdef QT_CORE_LIB
            _dllPtr.setFileName(_dllName.c_str());
            loadSuccess = _dllPtr.load();
#elif _MSC_VER
            _dllPtr = LoadLibraryA(_dllName.c_str());
            if (_dllPtr)  {
                loadSuccess = true;
            }
            else {
                loadSuccess = false;
            }
#else
            // Not yet implemented
#endif
        }
    }
    _isLoaded = loadSuccess;
    return loadSuccess;
}

bool DynamicLoadLibrary::unload() {
    bool unloadSuccess = false;
    if (_isLoaded) {
#ifdef QT_CORE_LIB
        unloadSuccess = _dllPtr.unload();
#elif _MSC_VER
        DWORD  UnloadDLLThreadID;
        HANDLE hThread =  CreateThread(nullptr, 0, unloadDll, &_dllPtr, 0, &UnloadDLLThreadID);
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        unloadSuccess = (_dllPtr) ? false:true;
#else
    // Not yet implemented
#endif
    }
    else {
        unloadSuccess = true;
    }    
    _isLoaded = !unloadSuccess;
    if (unloadSuccess) {
        _fcnPtrStore.clear();
    }
    return unloadSuccess;
}

void DynamicLoadLibrary::resolveSymbol(const std::string symbol) {
    MFunctionPointer fptr = nullptr;
    if (_isLoaded) {
        if (_fcnPtrStore.find(symbol) == _fcnPtrStore.end()) {
#ifdef QT_CORE_LIB
            fptr = _dllPtr.resolve(symbol.c_str());
#elif _MSC_VER
            fptr = reinterpret_cast<mFunctionPointer>(GetProcAddress(_dllPtr, symbol.c_str()));
#else
            // Not yet implemented
#endif
            _fcnPtrStore.emplace(symbol,fptr);
        }
    }
}

}
