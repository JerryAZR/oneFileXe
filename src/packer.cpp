#include "packer.h"
#include <QDir>
#include <QDataStream>
#include <QTextStream>
#include <QDirIterator>
#include <QProcess>
#include <QDebug>

/**
 * @brief pack
 *
 * Packs a directory into a single executable,
 * with "AppRun" being the entry point
 *
 * @param dirName   The application directory to pack
 * @param outName   Name of the output file
 * @return error code (0 if success)
 */
int packer::pack(const QString &dirName, const QString &outName, int compLvl,
                 const QString &loader, const QString &generator)
{
    QDir appDir(dirName);
    appDir.makeAbsolute();
    appDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    // Check if the directory itself exists
    if (!appDir.exists()) {
        qCritical() << "Error: Cannot find AppDir" << dirName;
        return packer::ERROR_NO_APPDIR;
    }

    // Check if the directory contains a launcher script
    if (!(appDir.exists("AppRun") || appDir.exists("AppRun.exe") || appDir.exists("AppRun.bat"))) {
        qCritical() << "Error:";
        qCritical() << "Cannot find program entry point";
        qCritical() << "The entry point can be one of {AppRun, AppRun.exe, AppRun.bat}";
        return packer::ERROR_NO_APPRUN;
    }

    // Create source directory for cmake project
    QDir cmakeSrcDir("src-ofx");

    // Copy project template (loader) to build directory
    if (not cpDirContent(loader, cmakeSrcDir.path())) {
        qCritical() << "Error: cannot create project";
        return packer::ERROR_CANT_CREATE_PROJECT;
    }

    // Copy AppDir to build directory
    if (not cpDirContent(dirName, cmakeSrcDir.filePath("AppDir"))) {
        return packer::ERROR_CANT_COPY_APPDIR;
    }

    // Create resource file
    QFile qrcFile(cmakeSrcDir.filePath("data.qrc"));
    qrcFile.open(QFile::WriteOnly);
    QTextStream qrcStream(&qrcFile);
    qrcStream << "<RCC><qresource prefix=\"/\">\n";

    // Add all files in AppDir as resource files
    QDirIterator it(appDir, QDirIterator::Subdirectories);
    QString qrcFileEntry("    <file>AppDir/%1</file>\n");
    while (it.hasNext()) {
        QString fname = it.next();
        QString relPath = appDir.relativeFilePath(fname);
        if (QFileInfo(fname).isFile()) {
            qrcStream << qrcFileEntry.arg(relPath);
        }
    }
    qrcStream << "</qresource></RCC>\n";
    qrcFile.close();

    // TODO: build project
    // Remove CMakeCache.txt, ignore error
    QFile::remove("build-ofx/CMakeCache.txt");
    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.setInputChannelMode(QProcess::ForwardedInputChannel);

    // CMake configuration
    QStringList confArgs;
    confArgs << "-S" << "src-ofx" << "-B" << "build-ofx"
              << "-DCMAKE_BUILD_TYPE=Release";
    if (generator.length() > 0) {
        confArgs << "-G" << generator;
    }
#ifdef Q_OS_WIN
    process.start("qt-cmake.bat", confArgs);
#else
    process.start("qt-cmake", confArgs);
#endif
    if (!process.waitForStarted()) {
        qCritical() << "cmake failed to start";
        qCritical() << process.errorString();
        qCritical() << "Using system PATH:" << qgetenv("PATH");
        return packer::ERROR_CMAKE_CONF_FAILED;
    }
    // The process should not timeout
    process.waitForFinished(-1);
    if (process.exitCode() != 0) {
        qCritical() << "CMake failed with exit code" << process.exitCode();
        return packer::ERROR_CMAKE_CONF_FAILED;
    }

    // CMake build
    QStringList buildArgs;
    buildArgs << "--build" << "build-ofx" << "--parallel";
    process.start("cmake", buildArgs);
    if (!process.waitForStarted()) {
        qCritical() << "cmake failed to start";
        qCritical() << process.errorString();
        qCritical() << "Using system PATH:" << qgetenv("PATH");
        return packer::ERROR_CMAKE_BUILD_FAILED;
    }
    // The process should not timeout
    process.waitForFinished(-1);
    if (process.exitCode() != 0) {
        qCritical() << "CMake build failed with exit code" << process.exitCode();
        return packer::ERROR_CMAKE_BUILD_FAILED;
    }

    // Move project
    QFile::remove(outName);
#ifdef Q_OS_WIN
    if (not QFile("build-ofx/app.exe").copy(outName)) {
#else
    if (not QFile("build-ofx/app").copy(outName)) {
#endif
        qCritical() << "Cannot create executable" << outName;
        return packer::ERROR_CANT_CREATE_EXE;
    }

    return packer::ERROR_NONE;
}

/**
 * @brief packer::cpDirContent
 *
 * Copy the CONTENTS of src to dest
 *
 * @param src:  source directory
 * @param dest: destination directory
 * @return true if successful, false otherwise
 */
bool packer::cpDirContent(const QString &src, const QString &dest)
{
    if (not QDir::current().mkpath(dest)) {
        qCritical() << "Failed to make destination directory" << dest;
        return false;
    }
    QDir srcDir(src);
    srcDir.makeAbsolute(); // simplify relativepath calculation
    QDir destDir(dest);
    srcDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    // If copying from internal resource system, update permissions
    bool setPermissions = src.startsWith(":");

    QDirIterator it(srcDir, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        QString relPath = srcDir.relativeFilePath(path); // path relative to src
        if (QFileInfo(path).isFile()) {
            // This is a file
            QString outPath = destDir.filePath(relPath);
            // Overwrite existing file
            // QFile::remove would fail if the target does not exist (harmless).
            QFile::remove(outPath);
            if (not QFile(path).copy(outPath)) {
                qCritical() << "Failed to copy file" << path
                            << "to" << outPath << "(" + relPath + ")";
                return false;
            } else if (setPermissions) {
                // Files copied from qt resource system have very limited
                // permissions
                QFile(outPath).setPermissions(
                            QFile::ReadOther | QFile::ReadOwner |
                            QFile::ReadGroup | QFile::WriteOwner
                            );
            }
        } else {
            // This is a directory
            if (not destDir.mkpath(relPath)) {
                qCritical() << "Failed to make directory"
                            << destDir.filePath(relPath);;
                return false;
            }
        }
    }
    return true;
}
