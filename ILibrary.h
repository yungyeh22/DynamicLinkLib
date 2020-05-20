#ifndef ILIBRARY_H
#define ILIBRARY_H

namespace Util {

/*
 * ILibrary is the library interface for a library (DLL).
 *
*/
class ILibrary
{

public:
    ILibrary();
    virtual ~ILibrary();

public:
    // Instruct to load the library
    virtual bool loadLib() = 0;
    // Instruct to iunload the library
    virtual bool unloadLib() = 0;
    // Instruct to parse symbols from a library
    virtual const char* libName() const = 0;
};

}

#endif // ILIBRARY_H
