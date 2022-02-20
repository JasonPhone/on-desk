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
    explicit MemoEditor(MemoObj *memo,
                        int posx = 300, int posy = 200,
                        QWidget *parent = nullptr);
    ~MemoEditor();
    void close_editor();
  signals:
    void signal_delete_memo(int handle);
    void signal_open_manager();

  private:
    void init_titlebar();
    void init_widgets();
    void init_memo();
    void init_connects();
    Ui::MemoEditor *ui;
    MemoObj *memo_;
    QTimer *save_timer;
    QAction *act_delete_memo;
    QAction *act_open_manager;
  private slots:
    void slot_close_clicked();
    void slot_save_all_data();
    void slot_memo_updated();
    void slot_save_title(const QString& title);
    void slot_bold();
    void slot_strike();
    void slot_underline();
    void slot_italic();
    void slot_list();
    // NOTE: hyperlink?
    void slot_image();
    // listen
    void slot_update_format(const QTextCharFormat &format);
    void slot_cursor_position_changed();
    void slot_open_manager();
    void slot_delete_memo();
    void slot_toggle_ontop(bool is_ontop);
};

#endif // MEMOEDITOR_H
