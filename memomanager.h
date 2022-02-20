#ifndef MEMOMANAGER_H
#define MEMOMANAGER_H

#include <QWidget>
#include <QList>
#include <QListWidget>
#include <QMap>
#include <QSystemTrayIcon>
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
    bool refresh_memo_list(const QList<MemoObj*> &memo_list);
    void init_connects();
    void init_tray_icon();
    // all configures
    struct MngrConf;
    MngrConf *conf_;
    QMap<int, MemoObj*> memos_;
    QMap<int, MemoEditor*> memo_editors_;
    QMap<int, QListWidgetItem*> memo_viewer_items_;
    QMap<int, MemoViewer*> memo_viewers_;
    QSystemTrayIcon *tray_icon_;
    QAction *act_restore_, *act_quit_;
    QMenu *tray_icon_menu_;

  signals:
    void signal_exit();
  private slots:
    void slot_close_clicked();
    void slot_exit();
    void slot_create_memo();
    void slot_memo_deleted(int handle);
    void slot_open_memo(int handle);
    void slot_show_manager();
    // for searchbar
    void slot_search_memos();

};
#endif // MEMOMANAGER_H
