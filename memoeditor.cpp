#include "memoeditor.h"
#include "ui_memoeditor.h"
#include "finetitle.h"

MemoEditor::MemoEditor(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MemoEditor)
{
  ui->setupUi(this);
  ui->editor_titlebar_->set_title_text("Memo", 14);
  ui->editor_titlebar_->set_button_type(TitleButtonType::ONLY_CLOSE);
}

MemoEditor::~MemoEditor()
{
  delete ui;
}
