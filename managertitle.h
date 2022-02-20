#ifndef MANAGERTITLE_H
#define MANAGERTITLE_H
#include <QWidget>
#include <QLabel>
#include <QPushButton>
namespace Ui {
  class ManagerTitle;
}
class ManagerTitle : public QWidget
{
    Q_OBJECT
  public:
    explicit ManagerTitle(QWidget *parent);
    ~ManagerTitle();
    void set_background_color(QColor clr);
    // This is total window width
    void set_window_border_width(int w);
  private:
    Ui::ManagerTitle *ui;
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    // Init
    void init_widgets();
    void init_connects();
    void load_style_sheet(const QString &sheet_name);
  signals:
    void signal_close_clicked();
    void signal_settings_clicked();
    void signal_create_clicked();
  protected slots:
    void slot_close_clicked();
    void slot_settings_clicked();
    void slot_create_clicked();
  private:
    QColor title_color_;
    // for dragging
    bool is_pressed_;
    QPoint start_pos_;
    int window_border_width_;
};


#endif // MANAGERTITLE_H
