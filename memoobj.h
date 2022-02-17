#ifndef MEMOOBJ_H
#define MEMOOBJ_H
#include <QFile>
#include <QDateTime>
#include <QTextDocument>
// pure inner class, static,
// only for memo file
class MemoObj
{
  public:
    ~MemoObj();
    // getters
    static MemoObj* init_memo_obj(const int handle,
                                  const QString memo_dir = "./");
    int handle();
    QString title();
    QDateTime date_created();
    QDateTime date_modified();
    void get_tags(QStringList &tags);
    QString file_path();
    QString meta_path();
    QString memo_dir();
    // setters
    void set_title(const QString new_title);
    void update_date_modified();
    void append_tag(QString new_tag);
    bool save_meta();
    bool load_meta();
  private:
    MemoObj(const int hdl, const QString memo_dir);
    struct MetaInfo;
    MetaInfo *meta_;
};

#endif // MEMOOBJ_H
