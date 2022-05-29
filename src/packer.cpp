#include "packer.h"
#include <QDir>
#include <QTemporaryDir>
#include <QDataStream>
#include <QTextStream>
#include <QDirIterator>
#include <QProcess>
#include <QDebug>
#include "cxxoptions.h"

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
                 const QString &loader, const QString &qtPath)
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

    // Create temporary build directory
    QTemporaryDir tmpDir;
    QDir buildDir(tmpDir.path());

    // Copy project template (loader) to build directory
    if (not cpDirContent(loader, buildDir.path())) {
        qCritical() << "Error: cannot create project";
        return packer::ERROR_CANT_CREATE_PROJECT;
    }

    // Copy AppDir to build directory
    if (not cpDirContent(dirName, buildDir.filePath("AppDir"))) {
        return packer::ERROR_CANT_COPY_APPDIR;
    }

    // Create resource file
    QFile qrcFile(buildDir.filePath("data.qrc"));
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

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.setInputChannelMode(QProcess::ForwardedInputChannel);

    // Run rcc
    QDir qtRoot(qtPath);
    QString rcc = qtRoot.absoluteFilePath("bin/rcc");
    QString data_qrc = buildDir.absoluteFilePath("data.qrc");
    QString data_cpp = buildDir.absoluteFilePath("data.cpp");
    QStringList rccArgs = {data_qrc, "-o", data_cpp};
    qInfo() << "Step (1/4):" << data_qrc << "-->" << data_cpp;
    process.start(rcc, rccArgs);
    if (!process.waitForStarted()) {
        qCritical() << "rcc failed to start";
        qCritical() << process.errorString();
        return packer::ERROR_BUILD_FAILED;
    }
    // The process should not timeout
    process.waitForFinished(-1);
    if (process.exitCode() != 0) {
        qCritical() << "rcc failed with exit code" << process.exitCode();
        return packer::ERROR_BUILD_FAILED;
    }

    // Compile data.o
    QString data_o = buildDir.absoluteFilePath("data.o");
    QString incPath1 = "-I" + qtRoot.absoluteFilePath("include");
    QString incPath2 = "-I" + qtRoot.absoluteFilePath("include/QtCore");
    QString incPath3 = "-I" + qtRoot.absoluteFilePath("mkspecs/win32-g++");
    QStringList gppArgs1 = {
        "-c", data_cpp, "-o", data_o, incPath1, incPath2, incPath3
    };
    qInfo() << "Step (2/4):" << data_cpp << "-->" << data_o;
    process.start("g++", gppArgs1 + cxx::CXXFLAGS);
    if (!process.waitForStarted()) {
        qCritical() << "g++ failed to start";
        qCritical() << process.errorString();
        return packer::ERROR_BUILD_FAILED;
    }
    // The process should not timeout
    process.waitForFinished(-1);
    if (process.exitCode() != 0) {
        qCritical() << "Failed to generate" << data_o;
        qCritical() << "g++ failed with exit code" << process.exitCode();
        return packer::ERROR_BUILD_FAILED;
    }

    // Compile main.o
    QString main_cpp = buildDir.absoluteFilePath("main.cpp");
    QString main_o = buildDir.absoluteFilePath("main.o");
    QStringList gppArgs2 = {
        "-c", main_cpp, "-o", main_o, incPath1, incPath2, incPath3
    };
    qInfo() << "Step (3/4):" << main_cpp << "-->" << main_o;
    process.start("g++", gppArgs2 + cxx::CXXFLAGS);
    if (!process.waitForStarted()) {
        qCritical() << "g++ failed to start";
        qCritical() << process.errorString();
        return packer::ERROR_BUILD_FAILED;
    }
    // The process should not timeout
    process.waitForFinished(-1);
    if (process.exitCode() != 0) {
        qCritical() << "Failed to generate" << main_o;
        qCritical() << "g++ failed with exit code" << process.exitCode();
        return packer::ERROR_BUILD_FAILED;
    }

    // Link executable
    QString libQt6Core = qtRoot.absoluteFilePath("lib/libQt6Core.a");
    QString libQt6BundledPcre2 =
            qtRoot.absoluteFilePath("lib/libQt6BundledPcre2.a");
    QStringList ldArgs = {
        "-o", outName, data_o, main_o, libQt6Core, libQt6BundledPcre2
    };
    qInfo() << "Step (4/4):" << main_o << data_o << "-->" << outName;
    process.start("g++", ldArgs + cxx::LFLAGS + cxx::LIBS);
    if (!process.waitForStarted()) {
        qCritical() << "g++ failed to start";
        qCritical() << process.errorString();
        return packer::ERROR_BUILD_FAILED;
    }
    // The process should not timeout
    process.waitForFinished(-1);
    if (process.exitCode() != 0) {
        qCritical() << "Failed to generate" << outName;
        qCritical() << "g++ failed with exit code" << process.exitCode();
        return packer::ERROR_CANT_CREATE_EXE;
    }

    qInfo() << "Successfully packed" << dirName << "into" << outName;
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
