#include "editortitle.h"
#include "ui_editortitle.h"
#include <QFile>
#include <QMouseEvent>
#include <QInputDialog>
const int kButtonWidth = 30, kButtonHeight = 30;
// publics
EditorTitle::EditorTitle(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::EditorTitle),
      title_color_(255, 255, 255),
      is_pressed_(false),
      window_border_width_(0)
{
  ui->setupUi(this);
//  init_widgets();
  init_connects();
//  load_style_sheet();
}

EditorTitle::~EditorTitle()
{
  delete ui;
}
void EditorTitle::set_background_color(QColor clr) {
  this->title_color_ = clr;
  QPalette pal = this->palette();
  pal.setColor(QPalette::Window, title_color_);
  this->setAutoFillBackground(true);
  this->setPalette(pal);
}
void EditorTitle::set_title_text(const QString &title_text, int font_size) {
  // size
  QFont font = ui->title_label->font();
  font.setPointSize(font_size);
  ui->title_label->setFont(font);
  // content
  ui->title_label->setText(title_text);
}
// protects
void EditorTitle::mouseDoubleClickEvent(QMouseEvent *e) {
  bool ok;
  QString old_title = ui->title_label->text();
  QString title_text =
      QInputDialog::getText(this->parentWidget(),
                            tr("Set title"), tr("Input the title"),
                            QLineEdit::Normal, old_title, &ok);
  slot_change_title(title_text);
  return QWidget::mouseDoubleClickEvent(e);
}
// privates
void EditorTitle::mousePressEvent(QMouseEvent *e) {
  is_pressed_ = true;
  start_pos_ = e->globalPosition().toPoint();
  return QWidget::mousePressEvent(e);
}
void EditorTitle::mouseMoveEvent(QMouseEvent *e) {
  if (is_pressed_) {
    QPoint displace = e->globalPosition().toPoint() - start_pos_;
    QPoint widget_pos = this->parentWidget()->pos();
    start_pos_ = e->globalPosition().toPoint();
    this->parentWidget()->move(widget_pos.x() + displace.x(),
                               widget_pos.y() + displace.y());
  }
  return QWidget::mouseMoveEvent(e);
}
void EditorTitle::mouseReleaseEvent(QMouseEvent *e) {
  is_pressed_ = false;
  return QWidget::mouseReleaseEvent(e);
}
void EditorTitle::init_widgets() {
  ui->button_close->setToolTip(tr("Close"));
}
void EditorTitle::init_connects() {
  connect(ui->button_close, &QPushButton::clicked,
          this, &EditorTitle::slot_close_clicked);
  connect(ui->button_top, &QPushButton::clicked,
          this, &EditorTitle::slot_toggle_ontop);
}
void EditorTitle::slot_change_title(const QString &title) {
  if (title.isEmpty()) return;
  this->set_title_text(title);
  emit signal_title_changed(title);
}
void EditorTitle::slot_close_clicked() {
//  qDebug() << "editor close clicked";
  emit signal_close_clicked();
}
void EditorTitle::slot_toggle_ontop() {
  qDebug() << "toggle on top";
  emit signal_toggle_ontop(ui->button_top->isChecked());
}
