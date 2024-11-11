#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

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

private:
    Ui::Main_Window *ui;
    QFileDialog *file_dialog;

private:
    void set_tool_bar();
    void open(const QUrl file_location);

private slots:
    void action_open_file_triggered();
};

#endif // MAIN_WINDOW_H
