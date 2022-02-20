#include "memoobj.h"
#include <QDateTime>
#include <QColor>
#include <QDir>
struct MemoObj::MetaInfo {
    /* format:
     * <configure>:<val1>,<val2>,<val3>;
     */
    int handle;
    QString title;
    // yyyy-MM-dd hh:mm:ss
    QDateTime date_created, date_modified;
    QStringList tags;
    QString memo_dir;
    QColor color;
    MetaInfo(const int hdl, const QString m_dir) {
      handle = hdl;
      title = "";
      date_created = QDateTime::currentDateTime();
      date_modified = date_created;
      tags.clear();
      memo_dir = m_dir;
      color = QColor(255, 255, 255);
    }
};

MemoObj::MemoObj(const int hdl, const QString memo_dir) {
  meta_ = new MetaInfo(hdl, memo_dir);
}
MemoObj::~MemoObj() {
  delete meta_;
}
// getters
MemoObj* MemoObj::init_memo_obj(const int handle,
                                const QString memo_dir) {
  qDebug() << "memoobj::initiating memo with handle " << handle << ", dir " << memo_dir;
  // need to store two files
  // thus need two file paths
  QDir memo_folder(memo_dir);
  // test the dir
  if (!memo_folder.exists()) {
    if (!memo_folder.mkpath(memo_dir)) {
      return nullptr;
    }
  }
  MemoObj *memo = new MemoObj(handle, memo_dir);
  qDebug() << "memoobj::dir ok";
  if (!memo->load_meta()) {
    qDebug() << "ERROR:memoobj::failed to load meta info";
    delete memo;
    return nullptr;
  }
  qDebug() << "memoobj::files created";
  // return memo ptr
  return memo;
}
int MemoObj::handle() {
  return meta_->handle;
}
QString MemoObj::title() {
  return meta_->title;
}
QDateTime MemoObj::date_created() {
  return meta_->date_created;
}
QDateTime MemoObj::date_modified() {
  return meta_->date_modified;
}
void MemoObj::get_tags(QStringList &tags) {
  tags = meta_->tags;
}
QString MemoObj::file_path() {
  return (meta_->memo_dir) + QString("%1").arg(meta_->handle) + ".html";
}
QString MemoObj::meta_path() {
  return (meta_->memo_dir) + QString("%1").arg(meta_->handle) + ".memo";
}
QString MemoObj::memo_dir() {
  return meta_->memo_dir;
}
// setters
void MemoObj::set_title(const QString new_title) {
  meta_->title = new_title;
}
void MemoObj::update_date_modified() {
  meta_->date_modified = QDateTime::currentDateTime();
}
void MemoObj::append_tag(QString new_tag) {
  meta_->tags.append(new_tag);
}
bool MemoObj::save_meta() {
  update_date_modified();
  QString meta_path =
      meta_->memo_dir + QString("%1").arg(meta_->handle) + ".memo";
  QFile meta(meta_path);
  if (!meta.open(QIODevice::WriteOnly)) return false;
  QTextStream meta_flow(&meta);
  // int handle;
  meta_flow << "handle:"
            << QString("%1").arg(meta_->handle) << ";";
  // QString title;
  meta_flow << "title:"
            << meta_->title << ";";
  // "yyyy-MM-dd hh:mm:ss"
  // QDateTime date_created, date_modified;
  meta_flow << "date_created:"
            << meta_->date_created.toString("yyyy-MM-dd hh:mm:ss") << ";";
  meta_flow << "date_modified:"
            << meta_->date_modified.toString("yyyy-MM-dd hh:mm:ss") << ";";
  // QStringList tags;
  meta_flow << "tags:";
  qDebug() << "memoobj::size of tags: " << meta_->tags.size();
  for (const auto& tag : meta_->tags) {
    meta_flow << tag << ",";
  }
  meta_flow << ";";
  // QString memo_dir;
  meta_flow << "memo_dir:"
            << meta_->memo_dir << ";";
  meta.close();
  return true;
}
bool MemoObj::load_meta() {
  // test the file
  qDebug() << "memoobj::testing meta";
  QString meta_path = this->meta_path();
  QFile meta(meta_path);
  qDebug() << "memoobj::try opening " << meta_path;
  if (meta.open(QIODevice::NewOnly)) {
    qDebug() << "memoobj::new memo meta";
    QTextStream meta_flow(&meta);
    meta_flow << "handle:" << this->handle() << ";"
              << "title:" << "" << ";"
              << "date_created:"
              << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << ";"
              << "date_modified:"
              << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << ";"
              << "tags:" << "" << ";"
              << "memo_dir:" << this->meta_->memo_dir << ";";
    meta.close();
    QString file_path = this->file_path();
    QFile file(file_path);
    qDebug() << "memoobj::try creating" << file_path;
    file.open(QIODevice::WriteOnly);
    file.close();
  } else {
    // init meta info if the memo exists
    meta.open(QIODevice::ReadOnly);
    QTextStream meta_flow(&meta);
    QString meta_str = meta_flow.readAll();
    meta.close();
    qDebug() << meta_str;
    QMap<QString, QString> meta_info;
    if (!meta_str.isEmpty()) {
      qDebug() << "memoobj::found existing meta, reading";
      QStringList entry = meta_str.split(";");
      for (auto& e : entry) {
        e = e.simplified();
        if (e.isEmpty()) continue;
//        qDebug() << e;
        int split_pos = e.indexOf(':');
        if (split_pos == -1) {
          qDebug() << "ERROR:memoobj::cannot read meta info: wrong configure format";
          return false;
        } else {
          QString key = e.first(split_pos).simplified();
          QString val = e.last(e.size() - split_pos - 1).simplified();
          meta_info[key] = val;
        }
      }
      if (meta_info.count("handle"))
        this->meta_->handle = meta_info["handle"].toInt();
      else {
        qDebug() << "ERROR:memoobj::no handle detected";
        return false;
      }
      if (meta_info.count("title"))
        this->meta_->title = meta_info["title"];
      else {
        qDebug() << "ERROR:memoobj::no title detected";
        return false;
      }
      // yyyy-MM-dd hh:mm:ss
      if (meta_info.count("date_created"))
        this->meta_->date_created =
            QDateTime::fromString(meta_info["date_created"], "yyyy-MM-dd hh:mm:ss");
      else {
        qDebug() << "ERROR:memoobj::no date_created detected";
        return false;
      }
      if (meta_info.count("date_modified"))
        this->meta_->date_modified =
            QDateTime::fromString(meta_info["date_modified"], "yyyy-MM-dd hh:mm:ss");
      else {
        qDebug() << "ERROR:memoobj::no date_modified detected";
        return false;
      }
      if (meta_info.count("memo_dir"))
        this->meta_->memo_dir = meta_info["memo_dir"];
      else {
        qDebug() << "ERROR:memoobj::no memo_dir detected";
        return false;
      }
      if (meta_info.count("tags")) {
        QStringList tag_list = meta_info["tags"].split(",");
        for (const auto& tag : tag_list) {
          if (tag.isEmpty()) continue;
          this->meta_->tags.append(tag);
        }
      } else {
        qDebug() << "ERROR:memoobj::no entry for tags";
        return false;
      }
      // NOTE: color is not considered
    } else {
      meta.open(QIODevice::WriteOnly);
      qDebug() << "memoobj::empty meta, new memo, writing defaults";
      QTextStream meta_flow(&meta);
      meta_flow << "handle:" << this->handle() << ";"
                << "title:" << "" << ";"
                << "date_created:"
                << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << ";"
                << "date_modified:"
                << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << ";"
                << "tags:" << "" << ";"
                << "memo_dir:" << this->meta_->memo_dir << ";";
    }
    meta.close();
  }
  return true;
}
