#ifndef PACKER_H
#define PACKER_H

#include <QString>

namespace packer {
    enum {
        ERROR_NONE = 0,
        ERROR_NO_APPDIR,
        ERROR_NO_APPRUN,
        ERROR_CANT_CREATE_EXE
    };

    int pack(const QString& dirName, const QString& outName, bool compress);
}

#endif // PACKER_H
