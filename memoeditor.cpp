#include "memoeditor.h"
#include "ui_memoeditor.h"
#include <QFile>
#include <QTextDocumentWriter>
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QWindow>
#include <QMimeDatabase>

MemoEditor::MemoEditor(MemoObj *memo, int posx, int posy, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MemoEditor),
      memo_(memo),
      save_timer(nullptr) {
  qDebug() << "editor with handle " << memo_->handle();
  ui->setupUi(this);
  init_titlebar();
  init_widgets();
  init_memo();
  init_connects();
  this->setGeometry(posx, posy, 300, 400);
  this->setWindowTitle(memo_->title());
//  this->show();
}

MemoEditor::~MemoEditor()
{
  delete ui;
  delete save_timer;
}
void MemoEditor::init_titlebar() {
  ui->editor_titlebar_->set_title_text(memo_->title());
}
void MemoEditor::init_widgets() {
  // appearance
  // ui->editor_titlebar_->setStyleSheet("border:0px");
  this->setWindowFlags(Qt::FramelessWindowHint |
                       Qt::WindowMinimizeButtonHint);
  QFile qss_file(":/resource/sty/pushbutton_style.qss");
  qss_file.open(QFile::ReadOnly);
  if (qss_file.isOpen()) {
    qDebug() << "style sheet opened";
    QString qss_content = this->styleSheet();
    qss_content += QLatin1String(qss_file.readAll());
    this->setStyleSheet(qss_content);
  }
  // format button
  ui->button_list->hide();
  // button more
  qDebug() << "editor::init more menu";
  QMenu *more_menu = new QMenu;
  act_delete_memo =
      more_menu->addAction(tr("&Delete memo"), this, &MemoEditor::slot_delete_memo);
  act_open_manager =
      more_menu->addAction(tr("&Open memo list"), this, &MemoEditor::slot_open_manager);
  ui->button_more->setMenu(more_menu);
  ui->button_more->setStyleSheet("QToolButton::menu-indicator{image:none;}");
  this->setWindowIcon(QIcon(":/resource/icon/main_icon_note.png"));
}
void MemoEditor::init_memo() {
  if (memo_ == nullptr) return;
  if (!memo_->load_meta()) return;
  // read and set contents
  QString f = memo_->file_path();
  qDebug() << "editor::opening file: " << f;
  if (!QFile::exists(f)) {
    qDebug() << "ERROR:editor::cannot find memo file";
    return;
  }
  QFile file(f);
  if (!file.open(QFile::ReadOnly)) {
    qDebug() << "ERROR:editor::cannot open memo file";
    return;
  }
  QByteArray data = file.readAll();
  QMimeDatabase db;
  const QString &mimeTypeName = db.mimeTypeForFileNameAndData(f, data).name();
  if (mimeTypeName == u"text/html") {
    auto encoding = QStringDecoder::encodingForHtml(data);
    QString str = QStringDecoder(encoding ? *encoding : QStringDecoder::Utf8)(data);
    QUrl fileUrl = f.startsWith(u':') ? QUrl(f) : QUrl::fromLocalFile(f);
    ui->editor_textarea_->document()->setBaseUrl(fileUrl.adjusted(QUrl::RemoveFilename));
    ui->editor_textarea_->setHtml(str);
#if QT_CONFIG(textmarkdownreader)
  } else if (mimeTypeName == u"text/markdown") {
    ui->editor_textarea_->setMarkdown(QString::fromUtf8(data));
#endif
  } else {
      ui->editor_textarea_->setPlainText(QString::fromUtf8(data));
  }
}
void MemoEditor::init_connects() {
  // QPushButton
  connect(ui->button_bold, &QPushButton::clicked,
          this, &MemoEditor::slot_bold);
  connect(ui->button_italic, &QPushButton::clicked,
          this, &MemoEditor::slot_italic);
  connect(ui->button_underline, &QPushButton::clicked,
          this, &MemoEditor::slot_underline);
  connect(ui->button_image, &QPushButton::clicked,
          this, &MemoEditor::slot_image);
  connect(ui->button_strike, &QPushButton::clicked,
          this, &MemoEditor::slot_strike);
  // QTextEdit
  connect(ui->editor_textarea_, &QTextEdit::textChanged,
          this, &MemoEditor::slot_memo_updated);
  connect(ui->editor_textarea_, &QTextEdit::cursorPositionChanged,
          this, &MemoEditor::slot_cursor_position_changed);
  connect(ui->editor_titlebar_, &EditorTitle::signal_close_clicked,
          this, &MemoEditor::slot_close_clicked);
  // titlebar
  connect(ui->editor_titlebar_, &EditorTitle::signal_title_changed,
          this, &MemoEditor::slot_save_title);
  // stay on top
  connect(ui->editor_titlebar_, &EditorTitle::signal_toggle_ontop,
          this, &MemoEditor::slot_toggle_ontop);
}
void MemoEditor::slot_memo_updated() {
  qDebug() << "editor::content changed at " << QDateTime::currentDateTime().toString("hh:mm:ss");
  if (memo_ == nullptr) return;
//  memo_->update_date_modified();
  if (save_timer == nullptr) {
    save_timer = new QTimer(this);
    save_timer->setSingleShot(true);
    connect(save_timer, &QTimer::timeout, this, &MemoEditor::slot_save_all_data);
  }
  save_timer->start(1500);
}
void MemoEditor::slot_save_all_data() {
  if (memo_ == nullptr) return;
  qDebug() << "editor::saving meta and file of memo " << memo_->handle();
  memo_->save_meta();
  QTextDocumentWriter writer(memo_->file_path());
  QTextEdit *textEdit = ui->editor_textarea_;
  bool success = writer.write(textEdit->document());
  if (success) {
      textEdit->document()->setModified(false);
      qDebug() << "success";
  } else {
    qDebug() << "failed";
  }
}
void MemoEditor::slot_save_title(const QString &title) {
  memo_->set_title(title);
  slot_memo_updated();
}
void MemoEditor::slot_bold() {
  QTextCharFormat fmt;
  fmt.setFontWeight(ui->button_bold->isChecked() ?
                      QFont::Bold : QFont::Normal);
  slot_update_format(fmt);
}
void MemoEditor::slot_strike() {
  QTextCharFormat fmt;
  fmt.setFontStrikeOut(ui->button_strike->isChecked());
  slot_update_format(fmt);
}
void MemoEditor::slot_underline() {
  QTextCharFormat fmt;
  fmt.setFontUnderline(ui->button_underline->isChecked());
  slot_update_format(fmt);
}
void MemoEditor::slot_italic() {
  QTextCharFormat fmt;
  fmt.setFontItalic(ui->button_italic->isChecked());
  slot_update_format(fmt);
}
void MemoEditor::slot_list() {
  // auto by markdown-like
}
void MemoEditor::slot_image() {
  QString image_path =
      QFileDialog::getOpenFileName(nullptr, "Open an image", "./", "*.png, *.jpg");
  if (!image_path.isEmpty()) {
    QTextImageFormat fmt;
    fmt.setName(image_path);
    // poor format
    QString image_src = "<img src=\""
                        + image_path
                        + "\" width=\"250\" >";
    ui->editor_textarea_->append(image_src);
  }
}
void MemoEditor::slot_update_format(const QTextCharFormat &format) {
  QTextCursor cursor = ui->editor_textarea_->textCursor();
  if (!cursor.hasSelection())
    cursor.select(QTextCursor::WordUnderCursor);
  cursor.mergeCharFormat(format);
  ui->editor_textarea_->mergeCurrentCharFormat(format);
}
void MemoEditor::slot_cursor_position_changed() {
  QTextCursor cursor = ui->editor_textarea_->textCursor();
  if (!cursor.hasSelection())
    cursor.select(QTextCursor::WordUnderCursor);
  QTextCharFormat format = cursor.charFormat();
  // is bold?
  ui->button_bold->setChecked(format.fontWeight() == QFont::Bold);
  // is italic?
  ui->button_italic->setChecked(format.fontItalic());
  // is underline?
  ui->button_underline->setChecked(format.fontUnderline());
  // is strikeout?
  ui->button_strike->setChecked(format.fontStrikeOut());
}
void MemoEditor::close_editor() {
  slot_close_clicked();
}
void MemoEditor::slot_close_clicked() {
  if (memo_ != nullptr)
    slot_save_all_data();
  if (save_timer != nullptr) {
    if (save_timer->timerId() != -1) save_timer->stop();
  }
  QWidget::close();
}
void MemoEditor::slot_open_manager() {
  qDebug() << "editor::calling for manager";
  emit signal_open_manager();
}
void MemoEditor::slot_delete_memo() {
  QMessageBox::StandardButton ok(
  QMessageBox::question(this, tr("Confirm delete"), tr("Delete this memo?"),
                       QMessageBox::Yes | QMessageBox::No));
  if (ok == QMessageBox::Yes) {
    slot_memo_updated();
    qDebug() << "editor::delete this memo";
    if (memo_ != nullptr)
      emit signal_delete_memo(memo_->handle());
  }
}
void MemoEditor::slot_toggle_ontop(bool is_ontop) {
  // using this->setWindowFlags() to toggle on/off top
  // will cause a window hide(), result in a flicker
  Qt::WindowFlags old_flags = this->windowFlags();
  QWindow* current_win = this->windowHandle();
  if (is_ontop) {
    current_win->setFlags(old_flags | Qt::WindowStaysOnTopHint);
  } else {
    current_win->setFlags(old_flags & ~Qt::WindowStaysOnTopHint);
  }
}
