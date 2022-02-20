#include "managertitle.h"
#include "ui_managertitle.h"
#include <QFile>
#include <QMouseEvent>
#include <QInputDialog>
const int kButtonWidth = 30, kButtonHeight = 30;
// publics
ManagerTitle::ManagerTitle(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ManagerTitle),
      title_color_(255, 255, 255),
      is_pressed_(false),
      window_border_width_(0)
{
  ui->setupUi(this);
//  init_widgets();
  init_connects();
//  load_style_sheet();
}

ManagerTitle::~ManagerTitle()
{
  delete ui;
}
void ManagerTitle::set_background_color(QColor clr) {
  this->title_color_ = clr;
  QPalette pal = this->palette();
  pal.setColor(QPalette::Window, title_color_);
  this->setAutoFillBackground(true);
  this->setPalette(pal);
}
// privates
void ManagerTitle::mousePressEvent(QMouseEvent *e) {
  is_pressed_ = true;
  start_pos_ = e->globalPosition().toPoint();
  return QWidget::mousePressEvent(e);
}
void ManagerTitle::mouseMoveEvent(QMouseEvent *e) {
  if (is_pressed_) {
    QPoint displace = e->globalPosition().toPoint() - start_pos_;
    QPoint widget_pos = this->parentWidget()->pos();
    start_pos_ = e->globalPosition().toPoint();
    this->parentWidget()->move(widget_pos.x() + displace.x(),
                               widget_pos.y() + displace.y());
  }
  return QWidget::mouseMoveEvent(e);
}
void ManagerTitle::mouseReleaseEvent(QMouseEvent *e) {
  is_pressed_ = false;
  return QWidget::mouseReleaseEvent(e);
}
void ManagerTitle::init_widgets() {
  ui->button_close->setToolTip(tr("Close"));
}
void ManagerTitle::init_connects() {
  connect(ui->button_close, &QPushButton::clicked,
          this, &ManagerTitle::slot_close_clicked);
  connect(ui->button_create, &QPushButton::clicked,
          this, &ManagerTitle::signal_create_clicked);
}
void ManagerTitle::slot_close_clicked() {
  qDebug() << "manager close clicked";
  emit signal_close_clicked();
}
void ManagerTitle::slot_settings_clicked() {
  emit signal_settings_clicked();
}
void ManagerTitle::slot_create_clicked() {
  emit signal_create_clicked();
}
