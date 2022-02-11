#ifndef FINETITLE_H
#define FINETITLE_H
#include <QWidget>
#include <QLabel>
#include <QPushButton>
enum class TitleButtonType {
  MIN = 0,      // minimize and close
  MIN_AND_MAX,  // minimize and maximize and close
  ONLY_CLOSE    // only close
};
class FineTitle : public QWidget
{
    Q_OBJECT
  public:
    // FineTitle object must be the subclass of a window,
    // so nullptr default value of parent is omitted
    explicit FineTitle(QWidget *parent, TitleButtonType);
    ~FineTitle();
    void set_background_color(QColor clr);
    void set_title_icon(const QString &icon_path, QSize icon_size = QSize(25, 25));
    void set_title_text(const QString &title_text, int font_size);
    void set_title_width(int w);
    void set_button_type(TitleButtonType button_type);
    // This is total window width
    void set_window_border_width(int w);
    // Maintain window position and size
    // for restoring after maximize & minimize
    void save_restore_info(const QPoint pos, const QSize sz);
    // Reference param type for receiving value
    void get_restore_info(QPoint &pos, QSize &sz);
  protected:
    // different behaviours
    void mouseDoubleClickEvent(QMouseEvent *e);
  private:
    // That's common behaviour
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    // Init
    void init_widgets();
    void init_connects();
    void load_style_sheet(const QString &sheet_name);
  signals:
    void signal_minimize_clicked();
    void signal_restore_clicked();
    void signal_maximize_clicked();
    void signal_close_clicked();
  protected slots:
    void slot_minimize_clicked();
    void slot_restore_clicked();
    void slot_maximize_clicked();
    void slot_close_clicked();
  private:
    QLabel *label_icon_, *label_title_text_;
    TitleButtonType button_type_;
    QPushButton *button_minimize_;
    QPushButton *button_restore_;
    QPushButton *button_maximize_;
    QPushButton *button_close_;
    QColor title_color_;
    // previous size and position
    QPoint restore_pos_;
    QSize restore_size_;
    // for dragging
    bool is_pressed_;
    QPoint start_pos_;
    int window_border_width_;
};

#endif // FINETITLE_H
