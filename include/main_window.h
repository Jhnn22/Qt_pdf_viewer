#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QHash>

class Pdf_Viewer_Widget;
class Event_Overlay_Widget;

class QVBoxLayout;
class QFileDialog;
class QLineEdit;
class QLabel;
class QAction;
class QPushButton;

namespace Ui {
class Main_Window;
}

class Main_Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_Window(QWidget *parent = nullptr);
    ~Main_Window();

    QString get_name();

public slots:
    void load_push_button_clicked();
    void page_line_edit_return_pressed(const QString &input_text);
    void action_prev_page_triggered();
    void action_next_page_triggered();
    void action_full_screen_triggered();
    void restore_from_full_screen();

signals:
    void current_widget_changed(const QString &name);

private:
    Ui::Main_Window *ui;
    QWidget *focused_widget;
    Pdf_Viewer_Widget *focused_pdf_viewer_widget;
    Event_Overlay_Widget *event_overlay_widget;
    QVBoxLayout *pdf_list_layout;
    QFileDialog *pdf_dialog;
    QLineEdit *page_line_edit;
    QLabel *total_page_label;
    QAction *action_prev_page, *action_next_page, *action_full_screen;
    int current_page_index, total_page_index;
    QHash<QString, QWidget*> hash;
    QString name;

    void set_pdf_list();
    void set_tool_bar();
    void set_connects();
    void open_pdf(const QUrl &url);
    void make_widget(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &name);
    void make_button(const QString &name);
    void set_name(const QString &name);
};

#endif // MAIN_WINDOW_H
