#include <QFile>
#include <QDir>
#include <QByteArray>
#include <QDataStream>
#include <QProcess>
#include <QCryptographicHash>
#include <QDirIterator>


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

    // Make a temporary directory
    QDir::temp().mkdir("ofx" + signature);
    QDir workDir(QDir::tempPath() + "/ofx" + signature);

    // Check magic file "onefilexe.ready"
    QFile readyFile(workDir.absoluteFilePath("onefilexe.ready"));
    if (not readyFile.exists()) {
        // Copy files from resource system to external filesystem
        QString root = ":/AppDir";
        QDir appDir(root);
        appDir.makeAbsolute();
        appDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

        QDirIterator it(appDir, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString path = it.next();
            QString relPath = appDir.relativeFilePath(path);
            QFile inFile(path);
            if (QFileInfo(path).isFile()) {
                // This is a file
                QString outPath = workDir.absoluteFilePath(relPath);
                // Skip if file already present
                if (!workDir.exists(relPath)) {
                    inFile.copy(outPath);
                }
            } else {
                // This is a directory
                workDir.mkpath(relPath);
            }
        }
        // Create the readyfile
        readyFile.open(QFile::WriteOnly);
        readyFile.close();
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
