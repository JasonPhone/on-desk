#ifndef MEMOVIEWER_H
#define MEMOVIEWER_H
#include "memoobj.h"
#include <QWidget>

namespace Ui {
  class MemoViewer;
}

class MemoViewer : public QWidget
{
    Q_OBJECT

  public:
    explicit MemoViewer(MemoObj* memo,
                        QWidget *parent);
    ~MemoViewer();

  private:
    Ui::MemoViewer *ui;
    MemoObj *memo_;
    bool init_memo();
    void init_connects();
    void init_widgets();
    void mouseDoubleClickEvent(QMouseEvent *e);
    void enterEvent(QEnterEvent *e);
    void leaveEvent(QEvent *e);
    QAction* act_open_memo;
    QAction* act_delete_memo;
  signals:
    void signal_delete_memo(int handle);
    void signal_open_memo(int handle);
  private slots:
    void slot_update_memo();
    void slot_delete_memo();
    void slot_open_memo();
};

#endif // MEMOVIEWER_H
