#ifndef MEMOMANAGER_H
#define MEMOMANAGER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MemoManager; }
QT_END_NAMESPACE

class MemoManager : public QWidget
{
    Q_OBJECT

  public:
    MemoManager(QWidget *parent = nullptr);
    ~MemoManager();

  private:
    Ui::MemoManager *ui;
};
#endif // MEMOMANAGER_H
