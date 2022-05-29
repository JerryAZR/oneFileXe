#ifndef PACKER_H
#define PACKER_H

#include <QString>

namespace packer {
    enum {
        ERROR_NONE = 0,
        ERROR_NO_APPDIR,
        ERROR_NO_APPRUN,
        ERROR_CANT_CREATE_EXE,
        ERROR_CANT_CREATE_PROJECT,
        ERROR_CANT_COPY_APPDIR,
        ERROR_BUILD_FAILED
    };

    int pack(const QString& dirName, const QString& outName, int compLvl,
             const QString& loader, const QString& qtPath);
    bool cpDirContent(const QString& src, const QString& dest);
}

#endif // PACKER_H
