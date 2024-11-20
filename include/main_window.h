#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class QVBoxLayout;
class QFileDialog;
class Pdf_Viewer_Widget;

namespace Ui {
class Main_Window;
}

class Main_Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_Window(QWidget *parent = nullptr);
    ~Main_Window();

private:
    Ui::Main_Window *ui;
    QVBoxLayout *file_list_layout;
    QFileDialog *file_dialog;

    void set_connects();
    void open(const QUrl file_location);
    void make_page(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &name);
    void make_button(const QString &name);

private slots:
    void load_push_button_clicked();

};

#endif // MAIN_WINDOW_H
