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
    ui->mdi_area->setOption(QMdiArea::DontMaximizeSubWindowOnActivation);   // 활성화된 서브 윈도우가 다른 서브 윈도우에 영향을 주지 않음

    set_menu_bar();

    this->showMaximized();
}

Main_Window::~Main_Window()
{
    delete ui;
}

void Main_Window::set_menu_bar(){
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
        Pdf_Viewer_Widget *pdf_viewer_widget = new Pdf_Viewer_Widget(file_location, this);
        QMdiSubWindow *sub_window = ui->mdi_area->addSubWindow(pdf_viewer_widget);
        sub_window->setAttribute(Qt::WA_DeleteOnClose);
        sub_window->setWindowTitle(file_location.fileName());
        sub_window->show();

        QWidget *widget = sub_window->widget();

        // 화면 모드 변경
        connect(pdf_viewer_widget, &Pdf_Viewer_Widget::toggle_push_button_clicked, sub_window, [this, sub_window, widget](const int changed_page_mode){
            if(changed_page_mode == SINGLE_PAGE){
                widget->setParent(nullptr);
                widget->showFullScreen();
                this->hide();
            }
            else{
                this->show();
                sub_window->layout()->addWidget(widget);
            }
        });
    }
    else{
        qDebug() << "failed to open";
    }
}
