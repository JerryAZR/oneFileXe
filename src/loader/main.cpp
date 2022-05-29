#include <QFile>
#include <QDir>
#include <QByteArray>
#include <QDataStream>
#include <QProcess>
#include <QCryptographicHash>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile source(QCoreApplication::applicationFilePath());
    source.open(QFile::ReadOnly);
    // Calculate the file signature
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(&source);
    QByteArray signature = hash.result()
            .toBase64(QByteArray::Base64UrlEncoding |
                      QByteArray::OmitTrailingEquals);
    // get offset
    long offset;
    source.reset();
    source.seek(source.size() - sizeof(offset));
    source.read((char*)&offset, sizeof(offset));
    // get to the payload
    source.reset();
    source.seek(offset);

    // Make a temporary directory
    QDir::temp().mkdir("ofx" + signature);
    QDir workDir(QDir::tempPath() + "/ofx" + signature);
    // Check magic file "onefilexe.ready"
    QFile readyFile(workDir.absoluteFilePath("onefilexe.ready"));
    if (not readyFile.exists()) {
        // Create the directory only if it does not already exist
        QDataStream fstream(&source);
        bool isFile;
        QString path;
        QByteArray content;
        QFile::Permission permissions;
        while (!fstream.atEnd()) {
            fstream >> isFile >> path;
            if (isFile) {
                QFile outFile(workDir.absoluteFilePath(path));
                // skip if file exists
                if (not outFile.exists()) {
                    // Create the file and write the content
                    fstream >> permissions;
                    fstream >> content;
                    outFile.open(QFile::WriteOnly);
                    outFile.write(qUncompress(content));
                    outFile.close();
                    outFile.setPermissions(permissions);
                }
            } else {
                if (path.length() > 0) {
                    // create the directory
                    workDir.mkpath(path);
                } else {
                    // end signature (isFile==false, path=="")
                    break;
                }
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
