#include "memoviewer.h"
#include "ui_memoviewer.h"

#include <QDateTime>
#include <QMimeDatabase>
#include <QMessageBox>
#include <QMenu>
MemoViewer::MemoViewer(MemoObj* memo, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MemoViewer),
      memo_(memo) {
  ui->setupUi(this);
  qDebug() << "viewer::init viewer of memo " << memo_->handle();
  init_widgets();
  init_connects();
  if (!init_memo()) {
    qDebug() << "ERROR:viewer:: viewer cannot init memo";
  }
}
MemoViewer::~MemoViewer() {
  delete ui;
  delete memo_watcher_;
}
int MemoViewer::handle() {
  if (memo_ == nullptr) return 0;
  return memo_->handle();
}
QString MemoViewer::memo_text() {
  return ui->label_title->text() + ui->textBrowser->document()->toPlainText();
}
void MemoViewer::mouseDoubleClickEvent(QMouseEvent *e) {
  qDebug() << "viewer::double clicked";
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
  if (memo_ == nullptr) return false;
  if (!memo_->load_meta()) return false;
  ui->label_title->setText(memo_->title());
  ui->label_date->setText(memo_->date_modified().toString("yy-MM-dd hh:mm:ss"));
  ui->button_more->hide();
  // read and set contents
  QString f = memo_->file_path();
  qDebug() << "viewer::viewer opening file: " << f;
  if (!QFile::exists(f)) {
    qDebug() << "ERROR:viewer:: viewer cannot find memo file";
    return false;
  }
  QFile file(f);
  if (!file.open(QFile::ReadOnly)) {
    qDebug() << "ERROR:viewer:: viewer cannot open memo file";
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
  // title and date_modified
  ui->label_date->setText(memo_->date_modified().toString("yyyy-MM-dd hh:mm:ss"));
  ui->label_title->setText(memo_->title());
  // TODO?: format, previous 5 lines
  // appearance
  ui->textBrowser->setAttribute(Qt::WA_TransparentForMouseEvents);
  // this->setStyleSheet("*:hover{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0.0 #ffd9aa, stop:0.5 #ffbb6e, stop:0.55 #feae42, stop:1.0 #fedb74);}");
  ui->textBrowser->setStyleSheet("background:transparent;border-width:0;border-style:outset");
  return true;
}
void MemoViewer::init_widgets() {
  QMenu *more_menu = new QMenu;
  act_open_memo = more_menu->addAction(tr("&Open memo"), this, &MemoViewer::slot_open_memo);
  act_delete_memo = more_menu->addAction(tr("&Delete memo"), this, &MemoViewer::slot_delete_memo);
  ui->button_more->setMenu(more_menu);
  ui->button_more->setStyleSheet("QToolButton::menu-indicator{image:none;}");
}
void MemoViewer::init_connects() {
  memo_watcher_ = new QFileSystemWatcher;
  memo_watcher_->addPaths({memo_->meta_path(), memo_->file_path()});
  connect(memo_watcher_, &QFileSystemWatcher::fileChanged,
          this, &MemoViewer::slot_memo_updated);
}
void MemoViewer::slot_delete_memo() {
  QMessageBox::StandardButton ok(
  QMessageBox::question(this, tr("Confirm delete"), tr("Delete this memo?"),
                       QMessageBox::Yes | QMessageBox::No));
  if (ok == QMessageBox::Yes) {
    qDebug() << "viewer::deleting memo " << memo_->handle() << " from viewer";
    emit signal_delete_memo(this->memo_->handle());
  }
}
void MemoViewer::slot_open_memo() {
  qDebug() << "viewer::opening memo " << memo_->handle() << " from viewer";
  emit signal_open_memo(this->memo_->handle());
}
void MemoViewer::slot_memo_updated() {
  qDebug() << "viewer::memo " << memo_->handle() << " updated, viewer refreshing";
  if (init_memo()) {
    qDebug() << "viewer::refreshing done";
  } else {
    qDebug() << "viewer::failed";
  }
}
