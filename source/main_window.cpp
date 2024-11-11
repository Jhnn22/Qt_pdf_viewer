#include "main_window.h"
#include "ui_main_window.h"

#include "pdf_viewer_widget.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QMdiSubWindow>

Main_Window::Main_Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Main_Window)
    , file_dialog(nullptr)
{
    ui->setupUi(this);

    set_tool_bar();
}

Main_Window::~Main_Window()
{
    delete ui;
}

void Main_Window::set_tool_bar(){
    connect(ui->action_open_file, &QAction::triggered, this, &Main_Window::action_open_file_triggered);
}

void Main_Window::action_open_file_triggered(){
    if(file_dialog == nullptr){
        file_dialog = new QFileDialog(this, tr("Pdf 불러오기"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));   // 기본 경로 : 다운로드
        file_dialog->setAcceptMode(QFileDialog::AcceptOpen);   // 열기 모드
        file_dialog->setMimeTypeFilters({"application/pdf"});  // pdf 문서만 표시
    }
    if(file_dialog->exec() == QDialog::Accepted){
        const QUrl target = file_dialog->selectedUrls().constFirst();
        if(target.isValid()){
            open(target);
        }
    }
}

void Main_Window::open(const QUrl file_location){
    if(file_location.isLocalFile()){
        pdf_viewer_widget = new Pdf_Viewer_Widget(file_location, this);
        sub_window = ui->mdi_area->addSubWindow(pdf_viewer_widget);
        sub_window->setAttribute(Qt::WA_DeleteOnClose);
        sub_window->show();
    }
    else{
        qDebug() << "failed to open";
    }
}
