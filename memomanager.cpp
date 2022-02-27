#include "memomanager.h"
#include "./ui_memomanager.h"
#include "memoviewer.h"
#include <QFile>
#include <QDir>
#include <QThread>
#include <QAction>
#include <QMenu>
#include <algorithm>
#include <vector>
struct MemoManager::MngrConf {
   QString memo_folder;
   QColor theme_color;
   MngrConf() {
     memo_folder = "";
     theme_color = QColor(255, 255, 255);
   }
};
MemoManager::MemoManager(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MemoManager) {
  ui->setupUi(this);
  //  init_titlebar();
  this->setWindowFlags(Qt::FramelessWindowHint |
                       Qt::WindowMinimizeButtonHint);
  this->setGeometry(800, 100, 350, 700);
  this->resize(300, 700);
  conf_ = new MngrConf();
  qDebug() << "manager::init manager";
  if (!init_manager()) {
    qDebug() << "ERROR:manager::falied to init manager";
  }
  qDebug() << "manager::folder: " << conf_->memo_folder;
  qDebug() << "manager::init titlebar";
  if (!init_titlebar()) {
    qDebug() << "ERROR:manager::falied to init titlebar";
  }
  qDebug() << "manager::init memos";
  if (!init_memos()) {
    qDebug() << "ERROR:manager:: falied to init memos";
  }
  qDebug() << "manager::init viewers";
  if (!init_viewers()) {
    qDebug() << "ERROR:manager:: falied to init viewers";
  }
  qDebug() << "manager::init connects";
  init_connects();
  qDebug() << "manager::init tray icon";
  init_tray_icon();
  qDebug() << "manager::all init done";
}

MemoManager::~MemoManager()
{
  delete ui;
  delete conf_;
  delete tray_icon_;
  delete tray_icon_menu_;
  delete act_quit_;
  delete act_restore_;
}
bool MemoManager::init_manager() {
  qDebug() << "manager::init manager, testing \"ondesk.conf\"";
  QFile conf("./ondesk.conf");
  if (conf.open(QIODevice::NewOnly)) {
    conf.close();
    qDebug() << "manager::found new conf";
    QString file_path = "./ondesk.conf";
    QFile file(file_path);
    qDebug() << "manager::creating " << file_path;
    file.open(QIODevice::WriteOnly);
    file.close();
  }
  // init conf if the configure file exists
  conf.open(QIODevice::ReadWrite);
  QTextStream conf_flow(&conf);
  QString conf_str = conf_flow.readAll();
  qDebug() << "manager::conf: " << conf_str;
  QMap<QString, QString> conf_info;
  if (conf_str.isEmpty()) {
    qDebug() << "manager::empty conf, new use, writing defaults";
    conf_flow << "memo_folder:./memo/;";
    conf_flow << "theme_color:255,255,255;";
    conf_str = "memo_folder:./memo/;theme_color:255,255,255;";
  }
  qDebug() << "manager::found existing conf, reading";
  QStringList entry = conf_str.split(";");
  for (auto& e : entry) {
    e = e.simplified();
    if (e.isEmpty()) continue;
    int split_pos = e.indexOf(":");
    if (split_pos == -1) {
      qDebug() << "ERROR:manager::cannot read conf info: wrong configure format";
      return false;
    } else {
      QString key = e.first(split_pos).simplified();
      QString val = e.last(e.size() - split_pos - 1).simplified();
      conf_info[key] = val;
    }
  }
  qDebug() << "manager::assigning configures";
  if (conf_info.count("memo_folder")) {
    conf_->memo_folder = conf_info["memo_folder"].simplified();
  } else {
    qDebug() << "manager::no memo_folder detected, using default ./memo/";
    return false;
  }
  // NOTE: color is to-be-implement
  qDebug() << "manager::conf reading done";
  conf.close();
  return true;
}
bool MemoManager::init_titlebar() {
  connect(ui->manager_titlebar_, &ManagerTitle::signal_close_clicked,
          this, &MemoManager::slot_close_clicked);
  return true;
}
// reading and init MemoObjs
bool MemoManager::init_memos() {
  if (conf_->memo_folder.isEmpty()) {
    qDebug() << "ERROR: manager cannot find memos, memo folder unset";
    return false;
  } else {
    // read all memo meta,
    // only filename to get handle
    const QString memo_folder = conf_->memo_folder;
    QDir memo_dir(memo_folder);
    if (!memo_dir.exists()) {
      qDebug() << "manager::memo folder not found (" << memo_folder << ")";
      if (!memo_dir.mkpath("../memo/")) {
        qDebug() << "manager::fail to create default folder";
        return false;
      }
    }
    memo_dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    memo_dir.setSorting(QDir::DirsLast);
    QFileInfoList memo_file_list = memo_dir.entryInfoList();
    int memo_cnt = 0;
    for (const auto& memo_file : memo_file_list) {
      if (memo_file.fileName().last(5) != ".memo") continue;
      qDebug() << "manager::found memo " << memo_file.fileName();
      int handle_len = memo_file.fileName().indexOf('.');
      int handle = memo_file.fileName().first(handle_len).toInt();
      qDebug() << "manager::handle: " << handle;
      if (handle != 0) {
        MemoObj *new_memo = MemoObj::init_memo_obj(handle, memo_folder);
        if (new_memo == nullptr) {
          qDebug() << "WARN: manager::cannot initialize MemoObj for this memo";
        } else {
          qDebug() << "manager::MemoObj initialized";
          memos_[handle] = new_memo;
          memo_cnt++;
        }
      }
    }
    qDebug() << "manager::found " << memo_cnt << " memo(s)";
    return true;
  }
}
// init viewers and add to scroll area
bool MemoManager::init_viewers() {
  auto memo_list = memos_.values();
  std::sort(memo_list.begin(), memo_list.end(),
            [](MemoObj* (&a), MemoObj* (&b)) {
              return !(a->date_modified() < b->date_modified());
            });
  return refresh_memo_list(memo_list);
}
bool MemoManager::refresh_memo_list(const QList<MemoObj*> &memo_list) {
  QList<int> handles;
  const auto viewers = memo_viewers_.values();
  for (auto viewer : viewers) handles.append(viewer->handle());
  for (auto handle : handles) {
    QListWidgetItem *itm = memo_viewer_items_[handle];
    ui->listWidget->removeItemWidget(itm);
    delete itm;
    memo_viewer_items_.remove(handle);
    MemoViewer *viewer = memo_viewers_[handle];
    delete viewer;
    memo_viewers_.remove(handle);
  }
  for (auto memo : memo_list) {
    MemoViewer *viewer = new MemoViewer(memo, ui->listWidget);
    // connect
    connect(viewer, &MemoViewer::signal_delete_memo,
            this, &MemoManager::slot_memo_deleted);
    connect(viewer, &MemoViewer::signal_open_memo,
            this, &MemoManager::slot_open_memo);
    QListWidgetItem *itm = new QListWidgetItem(ui->listWidget);
    itm->setBackground(QBrush(QColor(223, 223, 223)));
    itm->setSizeHint(QSize(20, 150));
    ui->listWidget->addItem(itm);
    ui->listWidget->setItemWidget(itm, viewer);
    memo_viewer_items_[memo->handle()] = itm;
    memo_viewers_[memo->handle()] = viewer;
  }
  return true;
}
void MemoManager::init_connects() {
  connect(ui->manager_titlebar_, &ManagerTitle::signal_create_clicked,
          this, &MemoManager::slot_create_memo);
  connect(ui->button_search, &QPushButton::clicked,
          this, &MemoManager::slot_search_memos);
}
void MemoManager::init_tray_icon() {
  // actions
  act_restore_ = new QAction(tr("&Restore"), this);
  act_quit_ = new QAction(tr("&Quit"), this);
  connect(act_restore_, &QAction::triggered,
          this, &MemoManager::slot_show_manager);
  connect(act_quit_, &QAction::triggered,
          this, &MemoManager::slot_exit);
  // build tray menu
  tray_icon_menu_ = new QMenu(this);
  tray_icon_menu_->addAction(act_restore_);
  tray_icon_menu_->addSeparator();
  tray_icon_menu_->addAction(act_quit_);
  tray_icon_ = new QSystemTrayIcon(this);
  // add
  tray_icon_->setContextMenu(tray_icon_menu_);
  // appearance
  tray_icon_->setToolTip(tr("OnDesk"));
  tray_icon_->setIcon(QIcon(":/resource/icon/main_icon_note.png"));
  tray_icon_->show();
}
void MemoManager:: slot_create_memo() {
  qDebug() << "manager::create new memo";
  int handle = 1;
  while (handle <= 5000 && memos_.count(handle)) handle++;
  qDebug() << "manager::create MemoObj with handle " << handle;
  MemoObj *tmp_memo = MemoObj::init_memo_obj(handle, conf_->memo_folder);
  if (tmp_memo == nullptr) {
    qDebug() << "manager::cannot create MemoObj";
  } else {
    qDebug() << "manager::created";
    memos_[handle] = tmp_memo;
    // refresh list
    auto memo_list = memos_.values();
    std::sort(memo_list.begin(), memo_list.end(),
              [](MemoObj* (&a), MemoObj* (&b)) {
      return !(a->date_modified() < b->date_modified());
    });
    refresh_memo_list(memo_list);
    // open editor
    slot_open_memo(handle);
  }
}
void MemoManager:: slot_memo_deleted(int handle) {
  // viewer
  if (memo_viewer_items_.count(handle) == 0) return;
  QListWidgetItem *itm = memo_viewer_items_[handle];
  if (itm == nullptr) return;
  ui->listWidget->removeItemWidget(itm);
  delete itm;
  memo_viewer_items_.remove(handle);
  MemoViewer *viewer = memo_viewers_[handle];
  delete viewer;
  memo_viewers_.remove(handle);
  // editor
  if (memo_editors_.count(handle)) {
    MemoEditor *editor = memo_editors_[handle];
    editor->close_editor();
    delete editor;
    memo_editors_.remove(handle);
  }
  if (memos_.count(handle)) {
    MemoObj *memo = memos_[handle];
    QFile::remove(memo->meta_path());
    QFile::remove(memo->file_path());
    delete memo;
    memos_.remove(handle);
  }
}
void MemoManager::slot_close_clicked() {
  // hide the window,
  // only close from systray
  hide();
}
void MemoManager::slot_exit() {
  this->hide();
  tray_icon_->hide();
  const auto all_editors = memo_editors_.values();
  for (auto editor : all_editors) {
    editor->close_editor();
  }
  QThread::sleep(3);
  close();
}
void MemoManager::slot_open_memo(int handle) {
  // already opened
  if (memo_editors_.count(handle)) {
    memo_editors_[handle]->show();
    return;
  }
  MemoObj *tmp_memo = nullptr;
  for (auto kv : memos_.toStdMap()) {
    if (kv.first == handle) {
      tmp_memo = kv.second;
      break;
    }
  }
  if (tmp_memo == nullptr) {
    qDebug() << "manager::memo with handle " << handle << " not found";
    return;
  }
  MemoEditor *new_editor = new MemoEditor(tmp_memo, 300, 200);
  if (new_editor == nullptr) {
    qDebug() << "manager::cannot create MemoEditor with handle " << handle;
    return;
  }
  memo_editors_[handle] = new_editor;
  connect(new_editor, &MemoEditor::signal_open_manager,
          this, &MemoManager::slot_show_manager);
  connect(new_editor, &MemoEditor::signal_delete_memo,
          this, &MemoManager::slot_memo_deleted);
  new_editor->show();
  qDebug() << "manager::editor created";
}
void MemoManager::slot_show_manager() {
  this->show();
}
// for searchbar
void MemoManager::slot_search_memos() {
  const QString qry = ui->edit_search->text();
  if (qry.isEmpty()) return;
  const double qlen = qry.length();
  QMap<int, int> weight;
  // sort on edit distance
  const auto memo_viewers = memo_viewers_.values();
  for (auto memo : memo_viewers) {
    if (memo == nullptr) continue;
    const QString memo_text = memo->memo_text();
    std::vector<std::vector<int>> ed;
    ed.resize(qlen);
    for (auto& r : ed) r.resize(memo_text.length());
    for (int i = 0; i < qlen; i++) {
      for (int j = 0; j < memo_text.length(); j++) {
        if (std::min(i, j) == 0) ed[i][j] = std::max(i, j);
        else {
          ed[i][j] = std::min(std::min(ed[i - 1][j] + 1, ed[i][j - 1] + 1),
                              ed[i - 1][j - 1] + (qry[i] != memo_text[j] ? 1 : 0));
        }
      }
    }
    weight[memo->handle()] = ed[qlen - 1][memo_text.length() - 1];
  }
  QList<int> handles = weight.keys();
  std::sort(handles.begin(), handles.end(),
            [&weight](const int &memo_a, const int &memo_b) {
              return weight[memo_a] < weight[memo_b];
            });
  QList<MemoObj*> memo_list;
  for (auto hdl : handles) {
    if (memos_.count(hdl)) memo_list.append(memos_[hdl]);
    qDebug() << hdl << ", " << weight[hdl];
  }
  refresh_memo_list(memo_list);
}
