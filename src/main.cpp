#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QDebug>
#include "packer.h"

/**
 * @brief main
 * @param argc
 * @param argv
 *
 * TODO: Add full translation
 *
 * @return
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString exeDir = QCoreApplication::applicationDirPath();

    QCommandLineParser parser;
    QCommandLineOption outOption(
                QStringList() << "o" << "output",
                a.translate("main", "Save packed executable as <file>"),
                a.translate("main", "file"),
                "out.exe");
    QCommandLineOption compressOption(
                QStringList() << "c" << "compress",
                a.translate("main", "Set compression level (not implemented)"),
                a.translate("main", "level"),
                "6");
    QCommandLineOption loaderOption(
                QStringList() << "loader",
                a.translate("main", "Set loader (default: built-in loader)"),
                a.translate("main", "loader"),
                ":/loader");
    QCommandLineOption qtOption(
                QStringList() << "qt-bin",
                a.translate("main", "Prepend <qt_path> to system PATH"),
                a.translate("main", "qt_path"),
                exeDir + "/Qt/bin"
                );
    QCommandLineOption compilerOption(
                QStringList() << "compiler-bin",
                a.translate("main", "Prepend <compiler_path> to system PATH"),
                a.translate("main", "compiler_path"),
                exeDir + "/MinGW/bin"
                );
    QCommandLineOption cmakeOption(
                QStringList() << "cmake-bin",
                a.translate("main", "Prepend <cmake_path> to system PATH"),
                a.translate("main", "cmake_path"),
                exeDir + "/CMake/bin"
                );
    QCommandLineOption generatorOption(
                QStringList() << "G" << "generator",
                a.translate("main", "Set CMake generator to <generator>"),
                a.translate("main", "generator"),
                ""
                );
    parser.addHelpOption();
    parser.addPositionalArgument(
                "AppDir",
                a.translate("main", "The application directory to pack."));

    parser.addOption(outOption);
    parser.addOption(compressOption);
    parser.addOption(loaderOption);
    parser.addOption(qtOption);
    parser.addOption(compilerOption);
    parser.addOption(cmakeOption);
    parser.addOption(generatorOption);

    parser.process(a);

    bool ok;
    int compLvl = parser.value(compressOption).toInt(&ok);
    if (!ok || compLvl > 9 || compLvl < 0) {
        compLvl = 6;
        qWarning() << "Unknown compression level. Using default (6)";
    }

    // Set environment
#ifdef Q_OS_WIN
    QChar envSeparator(';');
#else
    QChar envSeparator(':');
#endif
    QString sysPath;
    sysPath.append(QDir::toNativeSeparators(parser.value(qtOption)));
    sysPath.append(envSeparator);
    sysPath.append(QDir::toNativeSeparators(parser.value(compilerOption)));
    sysPath.append(envSeparator);
    sysPath.append(QDir::toNativeSeparators(parser.value(cmakeOption)));
    sysPath.append(envSeparator);
    sysPath.append(qgetenv("PATH"));
    qputenv("PATH", sysPath.toUtf8());

    // Auto configure CMake generator
    QString generator = parser.value(generatorOption);
    if (generator.length() == 0 &&
            parser.value(compilerOption).toLower().contains("mingw")) {
        generator = "MinGW Makefiles";
    }

    const QStringList args = parser.positionalArguments();

    if (args.length() > 0) {
        return packer::pack(args[0], parser.value(outOption), compLvl,
                parser.value(loaderOption), generator);
    } else {
        QTextStream outstream(stdout);
        outstream << parser.helpText();
    }
}
