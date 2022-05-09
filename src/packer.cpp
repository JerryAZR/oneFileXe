#include "packer.h"
#include <QDir>
#include <QDataStream>
#include <QDirIterator>
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
                 const QString &loader)
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
    if (!QFile(loader).copy(outName)) {
        qCritical() << "Error:";
        qCritical() << "Cannot create the output executable";
        return packer::ERROR_CANT_CREATE_EXE;
    }

    outFile.setPermissions(
                QFile::ReadOther | QFile::ReadOwner | QFile::ReadGroup |
                QFile::WriteOwner |
                QFile::ExeOther | QFile::ExeOwner | QFile::ExeGroup
                );
    // The baseSize is used to locate the start of data stream by the loader
    // It is appended to the file after we are done with everything else
    long baseSize = outFile.size();
    if (!outFile.open(QFile::Append | QFile::WriteOnly)) {
        qFatal("Failed to open file");
    }

    // We will use QDataStream to simplify the task
    QDataStream fstream(&outFile);
    fstream.setVersion(QDataStream::Qt_6_0);

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
            fstream << qCompress(content, compLvl);
        } else {
            // target is a directory (i.e., not a file)
            fstream << false << relPath;
        }
        inFile.close();
    }

    // Write terminator
    fstream << false << "";
    // Write offset
    outFile.write((char*)&baseSize, sizeof(baseSize));
    outFile.close();

    qInfo() << "Packed directory" << appDir.absolutePath();
    qInfo() << "file saved to" << outName;

    return packer::ERROR_NONE;
}
