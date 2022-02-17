#include "memoeditor.h"
#include "ui_memoeditor.h"
#include "finetitle.h"
#include <QFile>
#include <QTextDocumentWriter>
#include <QThread>

MemoEditor::MemoEditor(QWidget *parent, MemoObj *memo, int posx, int posy)
    : QWidget(parent),
      ui(new Ui::MemoEditor),
      memo_(memo),
      save_timer(nullptr) {
  ui->setupUi(this);
  init_titlebar();
  init_widgets();
  init_connects();
  this->setGeometry(posx, posy, 300, 400);
  this->show();
}

MemoEditor::~MemoEditor()
{
  delete ui;
  delete save_timer;
}
void MemoEditor::init_titlebar() {
  connect(ui->editor_titlebar_, &EditorTitle::signal_close_clicked,
          this, &MemoEditor::slot_close_clicked);
  connect(ui->editor_textarea_, &QTextEdit::textChanged,
          this, &MemoEditor::slot_memo_updated);
}
void MemoEditor::init_widgets() {
  // ui->editor_titlebar_->setStyleSheet("border:0px");
  this->setWindowFlags(Qt::FramelessWindowHint |
                       Qt::WindowMinimizeButtonHint);
  QFile qss_file(":/resource/sty/pushbutton_style.qss");
  qss_file.open(QFile::ReadOnly);
  if (qss_file.isOpen()) {
    qDebug() << "style sheet opened";
    QString qss_content = this->styleSheet();
    qss_content += QLatin1String(qss_file.readAll());
    this->setStyleSheet(qss_content);
  }
  ui->button_list->hide();

}
void MemoEditor::init_connects() {

}
void MemoEditor::slot_memo_updated() {
  qDebug() << "content changed";
  if (memo_ == nullptr) return;
  memo_->update_date_modified();
  if (save_timer == nullptr) {
    save_timer = new QTimer(this);
    save_timer->setSingleShot(true);
    connect(save_timer, &QTimer::timeout, this, &MemoEditor::slot_save_all_data);
  }
  save_timer->start(3000);
}
void MemoEditor::slot_save_all_data() {
  qDebug() << "saving meta and file of memo " << memo_->handle();
  memo_->save_meta();
  QTextDocumentWriter writer(memo_->file_path());
  QTextEdit *textEdit = ui->editor_textarea_;
  bool success = writer.write(textEdit->document());
  if (success) {
      textEdit->document()->setModified(false);
      qDebug() << "success";
  } else {
    qDebug() << "failed";
  }
}
void MemoEditor::slot_bold() {
  QTextCharFormat fmt;
  fmt.setFontWeight(ui->button_bold->isChecked() ?
                      QFont::Bold : QFont::Normal);
  slot_update_format(fmt);
}
void MemoEditor::slot_strike() {
  QTextCharFormat fmt;
  fmt.setFontStrikeOut(ui->button_strike->isChecked());
  slot_update_format(fmt);
}
void MemoEditor::slot_underline() {
  QTextCharFormat fmt;
  fmt.setFontUnderline(ui->button_underline->isChecked());
  slot_update_format(fmt);
}
void MemoEditor::slot_italic() {
  QTextCharFormat fmt;
  fmt.setFontItalic(ui->button_italic->isChecked());
  slot_update_format(fmt);
}
void MemoEditor::slot_list() {

}
void MemoEditor::slot_image() {

}
void MemoEditor::slot_update_format(const QTextCharFormat &format) {

}
void MemoEditor::slot_cursor_position_changed() {
}
void MemoEditor::close_editor() {
  slot_close_clicked();
}
void MemoEditor::slot_close_clicked() {
  if (memo_ != nullptr)
    slot_memo_updated();
  QWidget::hide();
  QThread::sleep(3000);
  QWidget::close();
}
