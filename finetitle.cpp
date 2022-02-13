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
  delete label_icon_;
  delete label_title_text_;
  delete button_close_;
  delete button_minimize_;
  delete button_maximize_;
  delete button_restore_;
}
void FineTitle::set_background_color(QColor clr) {
  this->title_color_ = clr;
  QPalette pal = this->palette();
  pal.setColor(QPalette::Window, title_color_);
  this->setAutoFillBackground(true);
  this->setPalette(pal);
}
void FineTitle::set_title_icon(const QString &icon_path, QSize icon_size) {
  QPixmap title_icon(icon_path);
  label_icon_->setPixmap(title_icon.scaled(icon_size));
}
void FineTitle::set_title_text(const QString &title_text, int font_size) {
  // size
  QFont font = label_title_text_->font();
  font.setPointSize(font_size);
  label_title_text_->setFont(font);
  // content
  label_title_text_->setText(title_text);
}
void FineTitle::set_title_width(int w) {
  this->setFixedWidth(w);
}
void FineTitle::set_button_type(TitleButtonType button_type) {
  button_type_ = button_type;
  // choose which buttons to hide
  switch (button_type_) {
    case TitleButtonType::MIN: {
        button_maximize_->setVisible(false);
        button_restore_->setVisible(false);
        break;
      }
    case TitleButtonType::MIN_AND_MAX: {
        button_restore_->setVisible(false);
        break;
      }
    case TitleButtonType::ONLY_CLOSE: {
        button_minimize_->setVisible(false);
        button_maximize_->setVisible(false);
        button_restore_->setVisible(false);
        break;
      }
    default:
      break;
  }
}
void FineTitle::set_window_border_width(int w) {
  window_border_width_ = w;
}
void FineTitle::save_restore_info(const QPoint pos, const QSize sz) {
  restore_pos_ = pos;
  restore_size_ = sz;
}
void FineTitle::get_restore_info(QPoint &pos, QSize &sz) {
  pos = restore_pos_;
  sz = restore_size_;
}
// protects
void FineTitle::mouseDoubleClickEvent(QMouseEvent *e) {
  return QWidget::mouseDoubleClickEvent(e);
}
// privates
void FineTitle::mousePressEvent(QMouseEvent *e) {
  // restore when maximized
  if (button_type_ == TitleButtonType::MIN_AND_MAX &&
      button_restore_->isVisible()) {
    emit signal_restore_clicked();
  }
  is_pressed_ = true;
  start_pos_ = e->globalPosition().toPoint();
  return QWidget::mousePressEvent(e);
}
void FineTitle::mouseMoveEvent(QMouseEvent *e) {
  if (is_pressed_) {
    QPoint displace = e->globalPosition().toPoint() - start_pos_;
    QPoint widget_pos = this->parentWidget()->pos();
    start_pos_ = e->globalPosition().toPoint();
    this->parentWidget()->move(widget_pos.x() + displace.x(),
                               widget_pos.y() + displace.y());
  }
  return QWidget::mouseMoveEvent(e);
}
void FineTitle::mouseReleaseEvent(QMouseEvent *e) {
  is_pressed_ = false;
  return QWidget::mouseReleaseEvent(e);
}
void FineTitle::init_widgets() {
  label_icon_ = new QLabel;
  label_title_text_ = new QLabel;
  // all set but show depending on buttontype
  button_close_     = new QPushButton;
  button_minimize_  = new QPushButton;
  button_maximize_  = new QPushButton;
  button_restore_   = new QPushButton;
  // size, objname, tooltip and layout
  // NOTE: this is not deleted in destructor
  //       will it cause memory leak?
  QHBoxLayout *title_layout = new QHBoxLayout(this);
  title_layout->setContentsMargins(0, 0, 0, 0);
  title_layout->setSpacing(3);
  title_layout->addWidget(label_icon_);
  title_layout->addWidget(label_title_text_);
  // minimize
  button_minimize_->setFixedSize(QSize(kButtonWidth, kButtonHeight));
  button_minimize_->setObjectName("buttonMinimize");
  button_minimize_->setToolTip(tr("Minimize"));
  title_layout->addWidget(button_minimize_);
  // restore
  button_restore_->setFixedSize(QSize(kButtonWidth, kButtonHeight));
  button_restore_->setObjectName("buttonRestore");
  button_restore_->setToolTip(tr("Restore"));
  title_layout->addWidget(button_restore_);
  // maximize
  button_maximize_->setFixedSize(QSize(kButtonWidth, kButtonHeight));
  button_maximize_->setObjectName("buttonMaximize");
  button_maximize_->setToolTip(tr("Maximize"));
  title_layout->addWidget(button_maximize_);
  // close
  button_close_->setFixedSize(QSize(kButtonWidth, kButtonHeight));
  button_close_->setObjectName("buttonClose");
  button_close_->setToolTip(tr("Close"));
  title_layout->addWidget(button_close_);
  // layout property
  title_layout->setContentsMargins(5, 0, 5, 0);
  title_layout->setSpacing(0);
  // titlebar property
  label_title_text_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  this->setFixedHeight(kTitleHeight);
  this->setWindowFlags(Qt::FramelessWindowHint);
}
void FineTitle::init_connects() {
    connect(button_minimize_, &QPushButton::clicked,
            this, &FineTitle::slot_minimize_clicked);
    connect(button_restore_, &QPushButton::clicked,
            this, &FineTitle::slot_restore_clicked);
    connect(button_maximize_, &QPushButton::clicked,
            this, &FineTitle::slot_maximize_clicked);
    connect(button_close_, &QPushButton::clicked,
            this, &FineTitle::slot_close_clicked);
}
// load qss
void FineTitle::load_style_sheet(const QString &sheet_name) {
  QFile qss_file(":/resource/sty/" + sheet_name + ".css");
  qss_file.open(QFile::ReadOnly);
  if (qss_file.isOpen()) {
    qDebug() << "style sheet opened";
    QString qss_content = this->styleSheet();
    qss_content += QLatin1String(qss_file.readAll());
    this->setStyleSheet(qss_content);
  }
}
// slots
void FineTitle::slot_minimize_clicked() {
  emit signal_minimize_clicked();
}
void FineTitle::slot_restore_clicked() {
  button_restore_->setVisible(false);
  button_maximize_->setVisible(true);
  emit signal_restore_clicked();
}
void FineTitle::slot_maximize_clicked() {
  button_maximize_->setVisible(false);
  button_restore_->setVisible(true);
  emit signal_maximize_clicked();
}
void FineTitle::slot_close_clicked() {
  emit signal_close_clicked();
}
