#include "memoeditortitle.h"
#include <QInputDialog>

MemoEditorTitle::MemoEditorTitle(QWidget *parent)
    : FineTitle(parent, TitleButtonType::ONLY_CLOSE) {

}

void MemoEditorTitle::mouseDoubleClickEvent(QMouseEvent *e) {
  bool ok;
  QString title_text = QInputDialog::getText(this->parentWidget(),
                                             tr("Set title"), tr("Input the title"),
                                             QLineEdit::Normal, tr("Memo"), &ok);
  slot_change_title(title_text);
  return FineTitle::mouseDoubleClickEvent(e);
}
void MemoEditorTitle::slot_change_title(const QString &title) {
  this->set_title_text(title, 14);
  emit signal_title_changed(title);
}
