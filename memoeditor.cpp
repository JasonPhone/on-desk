#include "memoeditor.h"
#include "ui_memoeditor.h"
#include "finetitle.h"

MemoEditor::MemoEditor(QWidget *parent, MemoObj *memo, int posx, int posy)
    : QWidget(parent),
      ui(new Ui::MemoEditor),
      memo_(memo) {
  ui->setupUi(this);
  init_titlebar();
  this->setWindowFlags(Qt::FramelessWindowHint |
                       Qt::WindowMinimizeButtonHint);
  this->setGeometry(posx, posy, 300, 400);

}

MemoEditor::~MemoEditor()
{
  delete ui;
}
void MemoEditor::init_titlebar() {
  connect(ui->editor_titlebar_, &EditorTitle::signal_close_clicked,
          this, &MemoEditor::slot_close_clicked);
  connect(ui->editor_textarea_, &QTextEdit::textChanged,
          this, &MemoEditor::slot_memo_updated);
}
void MemoEditor::slot_close_clicked() {
  if (memo_ != nullptr)
    slot_memo_updated();
  close();
}
void MemoEditor::slot_memo_updated() {
  //  qDebug() << "content changed";
  if (memo_ == nullptr) return;
  memo_->update_date_modified();
  memo_->save_meta();
  // TODO: save all text area content
}
