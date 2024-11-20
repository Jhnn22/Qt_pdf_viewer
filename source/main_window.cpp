#include "main_window.h"
#include "ui_main_window.h"

#include "pdf_viewer_widget.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>

Main_Window::Main_Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Main_Window)
    , file_dialog(nullptr)
{
    ui->setupUi(this);

    file_list_layout = qobject_cast<QVBoxLayout*>(ui->widget->layout());
    if(file_list_layout){
        file_list_layout->setContentsMargins(0, 0, 0, 0);
        file_list_layout->setAlignment(Qt::AlignTop);
    }

    set_connects();
}

Main_Window::~Main_Window()
{
    delete ui;
}

void Main_Window::set_connects(){
    connect(ui->load_push_button, &QPushButton::clicked, this, &Main_Window::load_push_button_clicked);
}

void Main_Window::load_push_button_clicked(){
    if(file_dialog == nullptr){
        file_dialog = new QFileDialog(this, tr("Pdf 불러오기"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));   // 기본 경로 : 다운로드
        file_dialog->setAcceptMode(QFileDialog::AcceptOpen);   // 열기 모드
        file_dialog->setMimeTypeFilters({"application/pdf"});  // pdf 문서만 표시
    }
    if(file_dialog->exec() == QDialog::Accepted){
        const QUrl url = file_dialog->selectedUrls().constFirst();
        if(url.isValid()){
            open_file(url);
        }
    }
}

void Main_Window::open_file(const QUrl url){
    if(url.isLocalFile()){
        Pdf_Viewer_Widget *pdf_viewer_widget = new Pdf_Viewer_Widget(url, this);
        make_page(pdf_viewer_widget, url.fileName());
        make_button(url.fileName());
    }
    else{
        qDebug() << "failed to open";
    }
}

void Main_Window::make_page(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &name){
    QWidget *page = new QWidget();
    page->setObjectName(name);
    QVBoxLayout *vertical_layout = new QVBoxLayout();
    vertical_layout->setContentsMargins(0, 0, 0, 0);
    vertical_layout->addWidget(pdf_viewer_widget);
    page->setLayout(vertical_layout);

    ui->stacked_widget->addWidget(page);

    connect(page->findChild<QPdfView*>(), &QPdfView::pageModeChanged, this, [this, page](QPdfView::PageMode changed_page_mode){
        if(changed_page_mode == QPdfView::PageMode::SinglePage){
            page->setParent(nullptr);
            page->showFullScreen();
            this->hide();
        }
        else{
            this->show();
            ui->stacked_widget->addWidget(page);
            ui->stacked_widget->setCurrentWidget(page);
        }
    });
}

void Main_Window::make_button(const QString &name){
    QPushButton *button = new QPushButton();
    button->setFixedSize(150, 30);
    button->setText(name);

    file_list_layout->insertWidget(file_list_layout->count() - 1, button);

    connect(button, &QPushButton::clicked, this, [this, name]{
        QWidget *widget = ui->stacked_widget->findChild<QWidget*>(name);
        if(widget){
            ui->stacked_widget->setCurrentWidget(widget);

            // 디버그 코드
            qDebug() << ui->stacked_widget->currentWidget();
            qDebug() << ui->stacked_widget->currentIndex();
        }
    });
}


