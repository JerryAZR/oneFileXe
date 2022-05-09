#include <QCoreApplication>
#include <QCommandLineParser>
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

    QCommandLineParser parser;
    QCommandLineOption outOption(
                QStringList() << "o" << "output",
                a.translate("main", "Save packed executable as <file>"),
                a.translate("main", "file"),
                "out.exe");
    QCommandLineOption compressOption(
                QStringList() << "c" << "compress",
                a.translate("main", "Set compression level (0-9)"),
                a.translate("main", "level"),
                "-1");
    QCommandLineOption loaderOption(
                QStringList() << "loader",
                a.translate("main", "Set loader (default: built-in loader)"),
                a.translate("main", "loader"),
                ":/loader/bin/base");
    parser.addHelpOption();
    parser.addPositionalArgument(
                "AppDir",
                a.translate("main", "The application directory to pack."));

    parser.addOption(outOption);
    parser.addOption(compressOption);
    parser.addOption(loaderOption);

    parser.process(a);

    bool ok;
    int compLvl = parser.value(compressOption).toInt(&ok);
    if (!ok || compLvl > 9 || compLvl < -1) {
        compLvl = -1;
        qWarning() << "Unknown compression level. Using default (-1)";
    }

    const QStringList args = parser.positionalArguments();

    if (args.length() > 0) {
        return packer::pack(args[0], parser.value(outOption), compLvl,
                parser.value(loaderOption));
    } else {
        QTextStream outstream(stdout);
        outstream << parser.helpText();
    }
}
