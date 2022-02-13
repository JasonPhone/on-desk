#ifndef EDITORTITLE_H
#define EDITORTITLE_H
#include <QWidget>
#include <QLabel>
#include <QPushButton>
namespace Ui {
  class EditorTitle;
}
class EditorTitle : public QWidget
{
    Q_OBJECT
  public:
    explicit EditorTitle(QWidget *parent);
    ~EditorTitle();
    void set_background_color(QColor clr);
    void set_title_text(const QString &title_text, int font_size = 14);
    // This is total window width
    void set_window_border_width(int w);
  private:
    Ui::EditorTitle *ui;
    // differs
    void mouseDoubleClickEvent(QMouseEvent *e);
    // That's common behaviour
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    // Init
    void init_widgets();
    void init_connects();
    void load_style_sheet(const QString &sheet_name);
  signals:
    void signal_close_clicked();
    void signal_title_changed(const QString &title);
  protected slots:
    void slot_close_clicked();
    void slot_change_title(const QString &title);
  private:
    QColor title_color_;
    // for dragging
    bool is_pressed_;
    QPoint start_pos_;
    int window_border_width_;
};

#endif // EDITORTITLE_H
