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
    parser.addHelpOption();
    parser.addPositionalArgument(
                "AppDir",
                a.translate("main", "The application directory to pack.")
                );

    parser.addOption(outOption);

    parser.process(a);

    const QStringList args = parser.positionalArguments();

    if (args.length() > 0) {
        return packer::pack(args[0], parser.value(outOption), true);
    } else {
        qInfo() << parser.helpText();
    }
}
