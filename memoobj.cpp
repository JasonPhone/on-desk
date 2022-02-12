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
  qDebug() << "initiating memo with handle " << handle << ", dir " << memo_dir;
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
  qDebug() << "dir ok";
  // test the file
  qDebug() << "testing meta";
  QString meta_path = memo_dir + QString("%1").arg(handle) + ".memo";
  QFile meta(meta_path);
  qDebug() << "try opening " << meta_path;
  if (meta.open(QIODevice::NewOnly)) {
    meta.close();
    qDebug() << "new memo meta";
    QString file_path = memo_dir + QString("%1").arg(handle) + ".html";
    QFile file(file_path);
    qDebug() << "try creating" << file_path;
    file.open(QIODevice::WriteOnly);
    file.close();
  } else {
    // init meta info if the memo exists
    meta.open(QIODevice::ReadOnly);
    QTextStream meta_flow(&meta);
    QString meta_str = meta_flow.readAll();
    qDebug() << meta_str;
    QMap<QString, QString> meta_info;
    if (!meta_str.isEmpty()) {
      qDebug() << "found existing meta, reading";
      QStringList entry = meta_str.split(";");
      for (auto& e : entry) {
        e = e.simplified();
        if (e.isEmpty()) continue;
        qDebug() << e;
        int split_pos = e.indexOf(":");
        if (split_pos == -1) {
          qDebug() << "ERROR: cannot read meta info: wrong configure format";
        } else {
          QString key = e.first(split_pos);
          QString val = e.last(e.size() - split_pos - 1);
          meta_info[key] = val;
        }
      }
      memo->meta_->handle = meta_info["handle"].toInt();
      memo->meta_->title = meta_info["title"];
      // yyyy-MM-dd hh:mm:ss
      memo->meta_->date_created =
          QDateTime::fromString(meta_info["date_created"], "yyyy-MM-dd hh:mm:ss");
      memo->meta_->date_modified =
          QDateTime::fromString(meta_info["date_modified"], "yyyy-MM-dd hh:mm:ss");
      memo->meta_->memo_dir = meta_info["memo_dir"];
      QStringList tag_list = meta_info["tags"].split(",");
      for (auto tag : tag_list) {
        if (tag.isEmpty()) continue;
        memo->meta_->tags.append(tag);
      }
      // NOTE: color is not considered
    } else {
      qDebug() << "empty meta, new memo";
    }
    meta.close();
  }
  qDebug() << "files created";
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
  //// "yyyy-MM-dd hh:mm:ss"
  // QDateTime date_created, date_modified;
  meta_flow << "date_created:"
            << meta_->date_created.toString("yyyy-MM-dd hh:mm:ss") << ";";
  meta_flow << "date_modified:"
            << meta_->date_modified.toString("yyyy-MM-dd hh:mm:ss") << ";";
  // QStringList tags;
  meta_flow << "tags:";
  qDebug() << "size of tags: " << meta_->tags.size();
  for (auto tag : meta_->tags) {
    meta_flow << tag << ",";
  }
  meta_flow << ";";
  // QString memo_dir;
  meta_flow << "memo_dir:"
            << meta_->memo_dir << ";";
  meta.close();
  return true;
}
