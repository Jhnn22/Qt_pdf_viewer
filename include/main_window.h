#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class Pdf_Viewer_Widget;
class Event_Overlay_Widget;

class QVBoxLayout;
class QFileDialog;
class QLineEdit;
class QLabel;
class QAction;


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

    Event_Overlay_Widget *event_overlay_widget;

    QVBoxLayout *pdf_list_layout;
    QFileDialog *pdf_dialog;
    QLineEdit *page_line_edit;
    QLabel *total_page_label;
    QAction *action_prev_page, *action_next_page, *action_toggle, *action_pointing, *action_drawing;

    int current_page_index, total_page_index;

    void set_pdf_list();
    void set_tool_bar();
    void set_connects();
    void open_pdf(const QUrl &url);
    void make_widget(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &name);
    void make_button(const QString &name);

private slots:
    void load_push_button_clicked();
    void page_line_edit_return_pressed(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &input_text);
    void action_prev_page_triggered(Pdf_Viewer_Widget *pdf_viewer_widget);
    void action_next_page_triggered(Pdf_Viewer_Widget *pdf_viewer_widget);
    void action_toggle_triggered(QWidget *widget);
};

#endif // MAIN_WINDOW_H
