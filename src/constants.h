#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>

namespace loader {
#ifdef Q_OS_WIN
    const unsigned int offset = (10 << 20);
#else
    const unsigned int offset = (4 << 20);
#endif
}

#endif // CONSTANTS_H
