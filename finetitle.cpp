#include "finetitle.h"
#include <QHBoxLayout>
#include <QFile>
#include <QMouseEvent>
const int kButtonWidth = 30, kButtonHeight = 30, kTitleHeight = 30;
// publics
FineTitle::FineTitle(QWidget *parent, TitleButtonType button_type)
    : QWidget{parent},
      title_color_(255, 255, 255),
      is_pressed_(false),
      button_type_(button_type),
      window_border_width_(0) {
  init_widgets();
  init_connects();
  // pass without suffix
  load_style_sheet("default_title_style");
}
FineTitle::~FineTitle() {
  // NOTE: Remember to delete!!!
}
void FineTitle::set_background_color(QColor clr) {

}
void FineTitle::set_title_icon(const QString &icon_path, QSize icon_size) {

}
void FineTitle::set_title_text(const QString &title_text, int font_size) {

}
void FineTitle::set_title_width(int w) {

}
void FineTitle::set_button_type(TitleButtonType button_type) {

}
void FineTitle::set_window_border_width(int w) {

}
void FineTitle::save_restore_info(const QPoint pos, const QSize sz) {

}
void FineTitle::get_restore_info(QPoint &pos, QSize &sz) {

}
// privates
// virtual void mouseDoubleClickEvent(QMouseEvent *e) = 0;
void FineTitle::mousePressEvent(QMouseEvent *e) {

}
void FineTitle::mouseMoveEvent(QMouseEvent *e) {

}
void FineTitle::mouseReleaseEvent(QMouseEvent *e) {

}
void FineTitle::init_widgets() {
  label_icon_ = new QLabel;
  label_title_text_ = new QLabel;

  button_close_ = new QPushButton;
  if (button_type_ == TitleButtonType::MIN) {
    button_minimize_ = new QPushButton;
    button_maximize_ = nullptr;
    button_restore_ = nullptr;
  } else if (button_type_ == TitleButtonType::MIN_AND_MAX) {
    button_minimize_ = new QPushButton;
    button_maximize_ = new QPushButton;
    button_restore_ = new QPushButton;
  } else {
    button_minimize_ = nullptr;
    button_maximize_ = nullptr;
    button_restore_ = nullptr;
  }
  // size, objname, tooltip and layout
  QHBoxLayout *title_layout = new QHBoxLayout(this);
  title_layout->addWidget(label_icon_);
  title_layout->addWidget(label_title_text_);
  if (button_minimize_ != nullptr) {
    button_minimize_->setFixedSize(QSize(kButtonWidth, kButtonHeight));
    button_minimize_->setObjectName("buttonMinimize");
    button_minimize_->setToolTip(tr("Minimize"));
    title_layout->addWidget(button_minimize_);
  }
  if (button_restore_ != nullptr) {
    button_restore_->setFixedSize(QSize(kButtonWidth, kButtonHeight));
    button_restore_->setObjectName("buttonRestore");
    button_restore_->setToolTip(tr("Restore"));
    title_layout->addWidget(button_restore_);
  }
  if (button_maximize_ != nullptr) {
    button_maximize_->setFixedSize(QSize(kButtonWidth, kButtonHeight));
    button_maximize_->setObjectName("buttonMaximize");
    button_maximize_->setToolTip(tr("Maximize"));
    title_layout->addWidget(button_maximize_);
  }
  if (button_close_ != nullptr) {
    button_close_->setFixedSize(QSize(kButtonWidth, kButtonHeight));
    button_close_->setObjectName("buttonClose");
    button_close_->setToolTip(tr("Close"));
    title_layout->addWidget(button_close_);
  }

  title_layout->setContentsMargins(5, 0, 5, 0);
  title_layout->setSpacing(0);

  label_title_text_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  this->setFixedHeight(kTitleHeight);
  this->setWindowFlags(Qt::FramelessWindowHint);
}
void FineTitle::init_connects() {
  if (button_minimize_ != nullptr) {
    connect(button_minimize_, &QPushButton::clicked,
            this, &FineTitle::slot_minimize_clicked);
  }
  if (button_restore_ != nullptr) {
    connect(button_restore_, &QPushButton::clicked,
            this, &FineTitle::slot_restore_clicked);
  }
  if (button_maximize_ != nullptr) {
    connect(button_maximize_, &QPushButton::clicked,
            this, &FineTitle::slot_maximize_clicked);
  }
  if (button_close_ != nullptr) {
    connect(button_close_, &QPushButton::clicked,
            this, &FineTitle::slot_close_clicked);
  }
}
void FineTitle::load_style_sheet(const QString &sheet_name) {

}
// slots
void FineTitle::slot_minimize_clicked() {

}
void FineTitle::slot_restore_clicked(){}
void FineTitle::slot_maximize_clicked(){}
void FineTitle::slot_close_clicked(){}
