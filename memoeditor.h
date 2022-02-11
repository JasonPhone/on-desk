#ifndef MEMOEDITOR_H
#define MEMOEDITOR_H

#include <QWidget>

namespace Ui {
  class MemoEditor;
}

class MemoEditor : public QWidget
{
    Q_OBJECT

  public:
    explicit MemoEditor(QWidget *parent = nullptr);
    ~MemoEditor();

  private:
    Ui::MemoEditor *ui;
};

#endif // MEMOEDITOR_H
