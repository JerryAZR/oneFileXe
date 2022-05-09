#include "packer.h"
#include <QDir>
#include <QDataStream>
#include <QDirIterator>
#include <QDebug>
#include "constants.h"

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
int packer::pack(const QString &dirName, const QString &outName, bool compress)
{
    QDir appDir(dirName);
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

    QFile outFile(outName);
    outFile.remove();
    if (!QFile(":/loader/bin/loader.exe").copy(outName)) {
        qCritical() << "Error:";
        qCritical() << "Cannot create the output executable";
        return packer::ERROR_CANT_CREATE_EXE;
    }

    outFile.setPermissions(
                QFile::ReadOther | QFile::ReadOwner | QFile::ReadGroup |
                QFile::WriteOwner |
                QFile::ExeOther | QFile::ExeOwner | QFile::ExeGroup
                );
    long baseSize = outFile.size();
    if (!outFile.open(QFile::Append | QFile::WriteOnly)) {
        qFatal("Failed to open file");
    }

    // Pad the output file to 4MB
    outFile.write(QByteArray(loader::offset - baseSize, '\0'));
    // We will use QDataStream to simplify the task
    QDataStream fstream(&outFile);
    fstream.setVersion(QDataStream::Qt_6_0);
    // Set compression flag
    fstream << compress;

    // Append all files
    QDirIterator it(appDir, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString fname = it.next();
        QString relPath = fname.mid(dirName.length());
        QFile inFile(fname);
        QByteArray content;
        if (inFile.open(QFile::ReadOnly)) {
            // target is a file
            fstream << true << relPath << inFile.permissions();
            content = inFile.readAll();
            if (inFile.pos() < inFile.size()) {
                qFatal("Input file too large");
            }
            fstream << (compress ? qCompress(content) : content);
        } else {
            // target is a directory (i.e., not a file)
            fstream << false << relPath;
        }
        inFile.close();
    }
    outFile.close();

    qInfo() << "Packed directory" << appDir;
    qInfo() << "file saved to" << outName;

    return packer::ERROR_NONE;
}
