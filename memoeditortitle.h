#ifndef MEMOEDITORTITLE_H
#define MEMOEDITORTITLE_H

#include "finetitle.h"

class MemoEditorTitle : public FineTitle
{
    Q_OBJECT
  public:
    MemoEditorTitle(QWidget *parent);
  private:
    void mouseDoubleClickEvent(QMouseEvent *e);
  signals:
    void signal_title_changed(const QString &title);
  private slots:
    void slot_change_title(const QString &title);
};

#endif // MEMOEDITORTITLE_H
