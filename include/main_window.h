#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class Pdf_Viewer_Widget;
class Event_Overlay_Widget;

class QLineEdit;
class QLabel;
class QAction;
class QVBoxLayout;
class QFileDialog;

namespace Ui {
class Main_Window;
}

class Main_Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_Window(QWidget *parent = nullptr);
    ~Main_Window();

signals:
    void current_widget_changed();

private:
    Ui::Main_Window *ui;

    QLineEdit *page_line_edit;
    QLabel *total_page_label;
    QAction *action_prev_page, *action_next_page,
            *action_zoom_out, *action_zoom_in,
            *action_toggle, *action_pointing, *action_drawing;
    QVBoxLayout *file_list_layout;
    QFileDialog *file_dialog;
    void set_tool_bar();
    void set_connects();
    void open_file(const QUrl &url);
    void make_page(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &name);
    void make_button(const QString &name);

    int current_page_index, total_page_index;
    qreal current_zoom_factor;
    const QVector<qreal> zoom_factors;
    int current_zoom_factor_index;
    int find_nearest_zoom_factor(const qreal current_zoom_factor);

    Event_Overlay_Widget *event_overlay_widget;

private slots:
    void load_push_button_clicked();
    void page_line_edit_return_pressed(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &input_text);
    void action_prev_page_triggered(Pdf_Viewer_Widget *pdf_viewer_widget);
    void action_next_page_triggered(Pdf_Viewer_Widget *pdf_viewer_widget);
    void action_zoom_out_triggered(Pdf_Viewer_Widget *pdf_viewer_widget);
    void action_zoom_in_triggered(Pdf_Viewer_Widget *pdf_viewer_widget);
    void action_toggle_triggered(QWidget *page);
};

#endif // MAIN_WINDOW_H
