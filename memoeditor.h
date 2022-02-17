#ifndef MEMOEDITOR_H
#define MEMOEDITOR_H

#include <QWidget>
#include <QTextCharFormat>
#include <QTimer>
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
                        int posx = 300, int posy = 200);
    ~MemoEditor();
    void close_editor();

  private:
    void init_titlebar();
    void init_widgets();
    void init_connects();
    Ui::MemoEditor *ui;
    MemoObj *memo_;
    QTimer *save_timer;
  private slots:
    void slot_close_clicked();
    void slot_save_all_data();
    void slot_memo_updated();
    void slot_bold();
    void slot_strike();
    void slot_underline();
    void slot_italic();
    void slot_list();
    void slot_image();
    // listen
    void slot_update_format(const QTextCharFormat &format);
    void slot_cursor_position_changed();

};

#endif // MEMOEDITOR_H
