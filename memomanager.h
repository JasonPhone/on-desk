#ifndef MEMOMANAGER_H
#define MEMOMANAGER_H

#include <QWidget>
#include <QList>
#include <QMap>
#include "memoobj.h"
#include "memoeditor.h"
#include "memoviewer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MemoManager; }
QT_END_NAMESPACE
enum class SearchType {
  BY_TIME = 0,
  BY_TEXT,
  BY_TAGS
};

class MemoManager : public QWidget
{
    Q_OBJECT

  public:
    MemoManager(QWidget *parent = nullptr);
    ~MemoManager();

  private:
    Ui::MemoManager *ui;
    // reading configures, ..., all previous work
    bool init_manager();
    bool init_titlebar();
    // reading and init MemoObjs
    bool init_memos();
    // init viewers and add to scroll area
    bool init_viewers();
    void init_connects();
    // open memo editor
    bool open_memo(int handle);
    // for searchbar
    void search_memos(const SearchType search_type,
                      const QString search_text);
    // all configures
    struct MngrConf;
    MngrConf *conf_;
    QMap<int, MemoObj*> memos_;
    QMap<int, MemoEditor*> memo_editors_;
    QMap<int, MemoViewer*> memo_viewers_;

  signals:
    void signal_memo_delete(int handle);
    void signal_exit();
  private slots:
    void slot_close_clicked();
    void slot_memo_created(int handle);
    void slot_memo_deleted(int handle);

};
#endif // MEMOMANAGER_H
