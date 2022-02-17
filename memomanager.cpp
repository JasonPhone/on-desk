#include "memomanager.h"
#include "./ui_memomanager.h"
#include "memoviewer.h"
#include <QFile>
#include <QDir>
#include <QThread>
struct MemoManager::MngrConf {
   QString memo_folder;
   QColor theme_color;
   MngrConf() {
     memo_folder = "";
     theme_color = QColor(255, 255, 255);
   }
};

MemoManager::MemoManager(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::MemoManager)
{
  ui->setupUi(this);
  //  init_titlebar();
  this->setWindowFlags(Qt::FramelessWindowHint |
                       Qt::WindowMinimizeButtonHint);
  this->setGeometry(600, 100, 350, 700);
  this->resize(300, 700);
  //  pos.setWidth(400);
  //  pos.setHeight(800);
  //  this->setGeometry(pos);
  conf_ = new MngrConf();
  qDebug() << "init manager";
  if (!init_manager()) {
    qDebug() << "ERROR: falied to init manager";
  }
  qDebug() << "init titlebar";
  qDebug() << "folder: " << conf_->memo_folder;
  if (!init_titlebar()) {
    qDebug() << "ERROR: falied to init titlebar";
  }
  qDebug() << "init memos";
  if (!init_memos()) {
    qDebug() << "ERROR: falied to init memos";
  }
  qDebug() << "init viewers";
  if (!init_viewers()) {
    qDebug() << "ERROR: falied to init viewers";
  }
  qDebug() << "init connects";

  qDebug() << "all init done";
}

MemoManager::~MemoManager()
{
  delete ui;
  delete conf_;
}

bool MemoManager::init_manager() {
  qDebug() << "init manager, testing \"ondesk.conf\"";
  QFile conf("./ondesk.conf");
  if (conf.open(QIODevice::NewOnly)) {
    conf.close();
    qDebug() << "new conf";
    QString file_path = "./ondesk.conf";
    QFile file(file_path);
    qDebug() << "try creating " << file_path;
    file.open(QIODevice::WriteOnly);
    file.close();
  }
  // init conf if the configure file exists
  conf.open(QIODevice::ReadWrite);
  QTextStream conf_flow(&conf);
  QString conf_str = conf_flow.readAll();
  qDebug() << conf_str;
  QMap<QString, QString> conf_info;
  if (!conf_str.isEmpty()) {
    qDebug() << "found existing conf, reading";
    QStringList entry = conf_str.split(";");
    for (auto& e : entry) {
      e = e.simplified();
      if (e.isEmpty()) continue;
      qDebug() << e;
      int split_pos = e.indexOf(":");
      if (split_pos == -1) {
        qDebug() << "ERROR: cannot read conf info: wrong configure format";
        return false;
      } else {
        QString key = e.first(split_pos).simplified();
        QString val = e.last(e.size() - split_pos - 1).simplified();
        conf_info[key] = val;
      }
    }
    qDebug() << "assigning configures";
    if (conf_info.count("memo_folder")) {
      conf_->memo_folder = conf_info["memo_folder"].simplified();
      qDebug() << conf_->memo_folder;
    } else {
      qDebug() << "no memo_folder detected, using default ./memo/";
      return false;
    }
    // NOTE: color is to-be-implement
    qDebug() << "conf reading done";
  } else {
    qDebug() << "empty conf, new use, writing defaults";
    conf_flow << "memo_folder:./memo/;";
    conf_flow << "theme_color:255,255,255;";
  }
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
      qDebug() << "memo folder not found (" << memo_folder << ")";
      if (!memo_dir.mkpath("../memo/")) {
        qDebug() << "fail to create default folder";
        return false;
      }
    }
    memo_dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    memo_dir.setSorting(QDir::DirsLast);
    QFileInfoList memo_file_list = memo_dir.entryInfoList();
    int memo_cnt = 0;
    for (const auto& memo_file : memo_file_list) {
      if (memo_file.fileName().last(5) != ".memo") continue;
      qDebug() << "found memo " << memo_file.fileName();
      int handle_len = memo_file.fileName().indexOf('.');
      int handle = memo_file.fileName().first(handle_len).toInt();
      qDebug() << "handle: " << handle;
      if (handle != 0) {
        MemoObj *new_memo = MemoObj::init_memo_obj(handle, memo_folder);
        if (new_memo == nullptr) {
          qDebug() << "WARN: cannot initialize MemoObj for this memo";
        } else {
          qDebug() << "MemoObj initialized";
          memos_[handle] = new_memo;
          memo_cnt++;
        }
      }
    }
    qDebug() << "found " << memo_cnt << " memo(s)";
    return true;
  }
}
// init viewers and add to scroll area
bool MemoManager::init_viewers() {
  for (auto memo : memos_.values()) {
    MemoViewer *viewer = new MemoViewer(memo, ui->listWidget);
    QListWidgetItem *itm = new QListWidgetItem(ui->listWidget);
    itm->setBackground(QBrush(QColor(223, 223, 223)));
    itm->setSizeHint(QSize(20, 150));
    ui->listWidget->addItem(itm);
    ui->listWidget->setItemWidget(itm, viewer);
  }
  return false;
}
void MemoManager::init_connects() {

}
// open memo editor
bool MemoManager::open_memo(int handle) {
  // already opened
  if (memo_editors_.count(handle)) {
    memo_editors_[handle]->show();
    return true;
  }
  MemoObj *tmp_memo = nullptr;
  for (auto kv : memos_.toStdMap()) {
    if (kv.first == handle) {
      tmp_memo = kv.second;
      break;
    }
  }
  if (tmp_memo == nullptr) {
    qDebug() << "memo with handle " << handle << " not found";
    return false;
  }
  MemoEditor *new_editor = new MemoEditor(this, tmp_memo, 300, 200);
  if (new_editor == nullptr) {
    qDebug() << "cannot create MemoEditor with handle " << handle;
    return false;
  }
  memo_editors_[handle] = new_editor;
  new_editor->show();
  qDebug() << "editor created";
  return true;
}
// for searchbar
void MemoManager:: search_memos(const SearchType search_type,
                                const QString search_text) {

}
void MemoManager:: slot_memo_created(int handle) {

}
void MemoManager:: slot_memo_deleted(int handle) {

}
void MemoManager::slot_close_clicked() {
  const auto all_editors = memo_editors_.values();
  for (auto editor : all_editors) {
    editor->close_editor();
  }
  // TODO: also hide the system tray
  hide();
  QThread::sleep(3);
  close();
}
