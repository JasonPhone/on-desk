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
    if (translator.load(":/i18n/" + baseName)) {
      a.installTranslator(&translator);
      break;
    }
  }
//  MemoManager w;
//  QColor clr(173, 153, 137);
//  QPalette pal = w.palette();
//  pal.setColor(QPalette::Window, clr);
//  w.setAutoFillBackground(true);
//  w.setPalette(pal);
//  w.show();
  MemoEditor w(nullptr, nullptr);
  w.show();
//  MemoObj *memo = MemoObj::init_memo_obj(2, "./memos/");
//  if (memo != nullptr) {
//    qDebug() << memo->handle() << ", " << memo->date_created().toString("yy-MM-dd hh:mm:ss");
//    memo->update_date_modified();
//    memo->save_meta();

//  } else
//    qDebug() << "got nullptr";
  return a.exec();
}
