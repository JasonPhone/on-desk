#include "memoviewer.h"
#include "ui_memoviewer.h"

#include <QDateTime>
#include <QMimeDatabase>
#include <QMenu>
MemoViewer::MemoViewer(MemoObj* memo, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MemoViewer),
  memo_(memo) {
  ui->setupUi(this);
  qDebug() << "init viewer of memo " << memo_->handle();
  if (!init_memo()) {
    qDebug() << "ERROR: viewer cannot init memo";
  }
  init_widgets();
  init_connects();
}
MemoViewer::~MemoViewer() {
  delete ui;
}
void MemoViewer::mouseDoubleClickEvent(QMouseEvent *e) {
  qDebug() << "double clicked";
  QWidget::mouseDoubleClickEvent(e);
  emit signal_open_memo(this->memo_->handle());
}
void MemoViewer::enterEvent(QEnterEvent *e) {
  ui->button_more->show();
  ui->label_date->hide();
  QWidget::enterEvent(e);
}
void MemoViewer::leaveEvent(QEvent *e) {
  ui->button_more->hide();
  ui->label_date->show();
  QWidget::leaveEvent(e);
}
bool MemoViewer::init_memo() {
  // TODO: use QFileSystemWatcher to watch the files
  assert(false);
  if (memo_ == nullptr) return false;
  if (!memo_->load_meta()) return false;
  ui->label_title->setText(memo_->title());
  ui->label_date->setText(memo_->date_modified().toString("yy-MM-dd hh:mm:ss"));
  ui->button_more->hide();
  // read and set contents
  QString f = memo_->file_path();
  qDebug() << "viewer opening file: " << f;
  if (!QFile::exists(f)) {
    qDebug() << "ERROR: viewer cannot find memo file";
    return false;
  }
  QFile file(f);
  if (!file.open(QFile::ReadOnly)) {
    qDebug() << "ERROR: viewer cannot open memo file";
    return false;
  }
  QByteArray data = file.readAll();
  QMimeDatabase db;
  const QString &mimeTypeName = db.mimeTypeForFileNameAndData(f, data).name();
  if (mimeTypeName == u"text/html") {
    auto encoding = QStringDecoder::encodingForHtml(data);
    QString str = QStringDecoder(encoding ? *encoding : QStringDecoder::Utf8)(data);
    QUrl fileUrl = f.startsWith(u':') ? QUrl(f) : QUrl::fromLocalFile(f);
    ui->textBrowser->document()->setBaseUrl(fileUrl.adjusted(QUrl::RemoveFilename));
    ui->textBrowser->setHtml(str);
#if QT_CONFIG(textmarkdownreader)
  } else if (mimeTypeName == u"text/markdown") {
    ui->textBrowser->setMarkdown(QString::fromUtf8(data));
#endif
  } else {
      ui->textBrowser->setPlainText(QString::fromUtf8(data));
  }
  // format, previous 5 lines
  while (ui->textBrowser->document()->lineCount() > 5) {
    ui->textBrowser->moveCursor(QTextCursor::End);
    QTextCursor tc = ui->textBrowser->textCursor();
    tc.select(QTextCursor::BlockUnderCursor);
    tc.removeSelectedText();
  }
  // appearance
  ui->textBrowser->setAttribute(Qt::WA_TransparentForMouseEvents);
  // this->setStyleSheet("*:hover{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0.0 #ffd9aa, stop:0.5 #ffbb6e, stop:0.55 #feae42, stop:1.0 #fedb74);}");
  ui->textBrowser->setStyleSheet("background:transparent;border-width:0;border-style:outset");
  return true;
}
void MemoViewer::init_widgets() {
  QMenu *more_menu = new QMenu;
//  more_menu->addMenu(tr("Open memo"));
//  more_menu->addMenu(tr("Delete memo"));
  act_open_memo = more_menu->addAction(tr("&Open memo"), this, &MemoViewer::slot_open_memo);
  act_delete_memo = more_menu->addAction(tr("&Delete memo"), this, &MemoViewer::slot_delete_memo);
  ui->button_more->setMenu(more_menu);
  ui->button_more->setStyleSheet("QToolButton::menu-indicator{image:none;}");
}
void MemoViewer::init_connects() {

}
void MemoViewer::slot_update_memo() {
  memo_->load_meta();
  init_memo();
}
void MemoViewer::slot_delete_memo() {
  qDebug() << "deleting memo " << memo_->handle();
  emit signal_delete_memo(this->memo_->handle());
}
void MemoViewer::slot_open_memo() {
  qDebug() << "opening memo " << memo_->handle();
  emit signal_open_memo(this->memo_->handle());
}
