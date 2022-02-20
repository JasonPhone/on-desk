#include "memomanager.h"
#include "memoeditor.h"
#include "memoobj.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for (const QString &locale : uiLanguages) {
    const QString baseName = "OnDesk_" + QLocale(locale).name();
    if (translator.load(":/resource/i18n/" + baseName)) {
      a.installTranslator(&translator);
      break;
    }
  }
  MemoManager mngr;
  qDebug() << "main::setting main icon";
  mngr.setWindowIcon(QIcon(":/resource/icon/main_icon_note.png"));
  mngr.show();
  return a.exec();
}
