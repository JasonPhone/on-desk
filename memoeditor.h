#ifndef MEMOEDITOR_H
#define MEMOEDITOR_H

#include <QWidget>
#include <QTextCharFormat>
#include "memoobj.h"

namespace Ui {
  class MemoEditor;
}

class MemoEditor : public QWidget
{
    Q_OBJECT

  public:
    explicit MemoEditor(QWidget *parent,
                        MemoObj *memo,
                        int posx, int posy);
    ~MemoEditor();

  private:
    void init_titlebar();
    void setupTextActions();
    Ui::MemoEditor *ui;
    MemoObj *memo_;
  private slots:
    void slot_close_clicked();
    void slot_memo_updated();
    void slot_bold();
    void slot_underline();
    void slot_italic();
    void slot_set_checked(bool checked);
    // listen
    void slot_current_format_changed(const QTextCharFormat &format);
    void slot_cursor_position_changed();

};

#endif // MEMOEDITOR_H
