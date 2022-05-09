#include "../constants.h"
#include <QFile>
#include <QDir>
#include <QByteArray>
#include <QDataStream>
#include <QProcess>
#include <QCryptographicHash>

int main(int argc, char *argv[])
{
    QFile source(argv[0]);
    source.open(QFile::ReadOnly);
    // Calculate the file signature
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(&source);
    QByteArray signature = hash.result()
            .toBase64(QByteArray::Base64UrlEncoding |
                      QByteArray::OmitTrailingEquals);
    // get to the payload
    source.reset();
    source.seek(loader::offset);

    // Make a temporary directory
    bool dirCheck = QDir::temp().mkdir("ofx" + signature);
    QDir workDir(QDir::tempPath() + "/ofx" + signature);
    if (dirCheck) {
        // Create the directory only if it does not already exist
        QDataStream fstream(&source);
        bool isFile;
        bool compress;
        QString path;
        QByteArray content;
        QFile::Permission permissions;
        // get compression flag
        fstream >> compress;
        while (!fstream.atEnd()) {
            fstream >> isFile >> path;
            if (isFile) {
                // Create the file and write the content
                fstream >> permissions;
                fstream >> content;
                QFile outFile(workDir.absoluteFilePath(path));
                outFile.open(QFile::WriteOnly);
                // TODO: verify integrity
                outFile.write(compress ? qUncompress(content) : content);
                outFile.close();
                outFile.setPermissions(permissions);
            } else {
                // create the directory
                workDir.mkpath(path);
            }
        }
    } else {
        // pass
    }

    // Run the application
    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.setInputChannelMode(QProcess::ForwardedInputChannel);
    QStringList args;
    for (int i = 0; i < argc-1; i++) {
        args << argv[i+1];
    }
    QString cmd = QDir::toNativeSeparators(workDir.absoluteFilePath("AppRun"));
    process.start(cmd, args);
    if (!process.waitForStarted()) {
        qCritical() << "Application failed to start";
        qCritical() << process.errorString();
        return -1;
    }

    // The process should not timeout
    process.waitForFinished(-1);

    return process.exitCode();
}
