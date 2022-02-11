#include "memoobj.h"
#include <QDateTime>
#include <QColor>
#include <QDir>
struct MemoObj::MetaInfo {
    int handle;
    QString title;
    QDateTime date_created, date_modified;
    QStringList tags;
    QDir file_path;
    QColor color;
    MetaInfo(const int hdl, const QString path) {
      handle = hdl;
      title = "";
      date_created = QDateTime::currentDateTime();
      date_modified = date_created;
      tags.clear();
      file_path = QDir(path);
      color = QColor(255, 0, 255);
    }
};

MemoObj::MemoObj(const int hdl, const QString file_path) {
  // file path is initialized here
  meta_ = new MetaInfo(hdl, file_path);

}
MemoObj::~MemoObj() {
  delete meta_;
}
// getters
MemoObj* MemoObj::get_memo_obj(const int handle, const QString memo_dir) {
  // TODO: need to store two files!!!!!!!
  //       thus need two file paths
  QString tmp_path = memo_dir + QString("%1").arg(handle) + ".memo";
  QDir memo_path = QDir(tmp_path);
  if (!memo_path.exists()) return nullptr;
  MemoObj *memo = new MemoObj(handle, tmp_path);
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
  return "";
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
