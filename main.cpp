#include "memomanager.h"

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
    if (translator.load(":/i18n/" + baseName)) {
      a.installTranslator(&translator);
      break;
    }
  }
  MemoManager w;
  QColor clr(173, 153, 137);
  QPalette pal = w.palette();
  pal.setColor(QPalette::Window, clr);
  w.setAutoFillBackground(true);
  w.setPalette(pal);
  w.show();
  return a.exec();
}
