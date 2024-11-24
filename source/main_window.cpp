#include "main_window.h"
#include "ui_main_window.h"

#include "pdf_viewer_widget.h"
#include "event_overlay_widget.h"

#include <QLineEdit>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPointF>

Main_Window::Main_Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Main_Window)
    , pdf_dialog(nullptr)
    , event_overlay_widget(new Event_Overlay_Widget(nullptr))
{
    ui->setupUi(this);


    set_pdf_list();
    set_tool_bar();
    set_connects();
}

Main_Window::~Main_Window()
{
    delete ui;
}

void Main_Window::set_pdf_list(){
    pdf_list_layout = qobject_cast<QVBoxLayout*>(ui->widget->layout());
    if(pdf_list_layout){
        pdf_list_layout->setContentsMargins(0, 0, 0, 0);
        pdf_list_layout->setAlignment(Qt::AlignTop);
    }
}

void Main_Window::set_tool_bar(){
    // 아래 정렬을 위한 스페이서
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 페이지
    QWidget *widget = new QWidget(this);
    QVBoxLayout *vertical_layout = new QVBoxLayout(widget);
    vertical_layout->setContentsMargins(0, 0, 0, 0);

    page_line_edit = new QLineEdit(this);
    page_line_edit->setFixedSize(30, 30);
    page_line_edit->setFrame(false);
    page_line_edit->setAlignment(Qt::AlignCenter);
    page_line_edit->setToolTip("특정 페이지 숫자로 이동");

    total_page_label = new QLabel(this);
    total_page_label->setFixedSize(20, 20);
    total_page_label->setAlignment(Qt::AlignCenter);
    total_page_label->setToolTip("이 PDF의 총 페이지 수");

    vertical_layout->addWidget(page_line_edit, 0, Qt::AlignCenter);
    vertical_layout->addWidget(total_page_label, 0, Qt::AlignCenter);

    action_prev_page = new QAction(this);
    action_prev_page->setText("▲");
    action_prev_page->setToolTip("이전 페이지로 이동");

    action_next_page = new QAction(this);
    action_next_page->setText("▼");
    action_next_page->setToolTip("다음 페이지로 이동");

    // 페인트 이벤트
    action_toggle = new QAction(this);
    action_toggle->setText("/");
    action_toggle->setToolTip("강조 기능 온오프");

    action_pointing = new QAction(this);
    action_pointing->setText("P");
    action_pointing->setToolTip("포인터");

    action_drawing = new QAction(this);
    action_drawing->setText("D");
    action_drawing->setToolTip("드로잉");

    // 순서에 맞게 툴 바에 추가
    ui->tool_bar->addWidget(spacer);
    ui->tool_bar->addWidget(widget);
    ui->tool_bar->addAction(action_prev_page);
    ui->tool_bar->addAction(action_next_page);
    ui->tool_bar->addSeparator();
    ui->tool_bar->addAction(action_toggle);
    ui->tool_bar->addAction(action_pointing);
    ui->tool_bar->addAction(action_drawing);
}

void Main_Window::set_connects(){
    // 파일 목록
    connect(ui->load_push_button, &QPushButton::clicked, this, &Main_Window::load_push_button_clicked);

    // 툴바
    // 초기 정보 설정
    connect(this, &Main_Window::current_widget_changed, this, [this](){
        Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
        if(pdf_viewer_widget){
            current_page_index = pdf_viewer_widget->get_current_page_index();
            page_line_edit->setText(QString::number(current_page_index + 1));
            total_page_index = pdf_viewer_widget->get_total_page_index();
            total_page_label->setText(QString::number(total_page_index + 1));

            qDebug() << "현재 페이지:" << current_page_index + 1;
            qDebug() << "전체 페이지:" << total_page_index + 1;
        }
    });
    // 페이지
    connect(page_line_edit, &QLineEdit::returnPressed, this, [this](){
        Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
        if(pdf_viewer_widget){
            QString input_text = page_line_edit->text();
            page_line_edit_return_pressed(pdf_viewer_widget, input_text);
        }
    });
    connect(action_prev_page, &QAction::triggered, this, &Main_Window::action_prev_page_triggered);
    connect(action_next_page, &QAction::triggered, this, &Main_Window::action_next_page_triggered);
    // 페인트 이벤트
    connect(action_toggle, &QAction::triggered, this, [this](){
        QWidget *widget = ui->stacked_widget->currentWidget();
        if(widget){
            action_toggle_triggered(widget);
        }
    });
    connect(action_pointing, &QAction::triggered, this, [this](){
        event_overlay_widget->set_paint_mode(POINTING);
    });
    connect(action_drawing, &QAction::triggered, this, [this](){
        event_overlay_widget->set_paint_mode(DRAWING);
    });
}

Pdf_Viewer_Widget *Main_Window::get_current_pdf_viewer_widget(){
    return ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
}

void Main_Window::load_push_button_clicked(){
    if(pdf_dialog == nullptr){
        pdf_dialog = new QFileDialog(this, tr("Pdf 불러오기"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));   // 기본 경로 : 다운로드
        pdf_dialog->setAcceptMode(QFileDialog::AcceptOpen);   // 열기 모드
        pdf_dialog->setMimeTypeFilters({"application/pdf"});  // pdf 문서만 표시
    }
    if(pdf_dialog->exec() == QDialog::Accepted){
        const QUrl url = pdf_dialog->selectedUrls().constFirst();
        if(url.isValid()){
            open_pdf(url);
        }
    }
}

void Main_Window::open_pdf(const QUrl &url){
    if(url.isLocalFile()){
        Pdf_Viewer_Widget *pdf_viewer_widget = new Pdf_Viewer_Widget(url, this);
        connect(pdf_viewer_widget, &Pdf_Viewer_Widget::update_page_info, this, [this, pdf_viewer_widget](){
            current_page_index = pdf_viewer_widget->get_current_page_index();
            qDebug() << "현재 페이지:" << current_page_index + 1;
            page_line_edit->setText(QString::number(current_page_index + 1));
        });

        make_widget(pdf_viewer_widget, url.fileName());
        make_button(url.fileName());
    }
    else{
        qDebug() << "failed to open";
    }
}

void Main_Window::make_widget(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &name){
    QWidget *widget = new QWidget(ui->stacked_widget);
    widget->setObjectName(name);
    QStackedLayout *stacked_layout = new QStackedLayout(widget);
    stacked_layout->setStackingMode(QStackedLayout::StackAll);
    stacked_layout->setContentsMargins(0, 0, 0, 0);
    stacked_layout->addWidget(pdf_viewer_widget);
    widget->setLayout(stacked_layout);

    ui->stacked_widget->addWidget(widget);
    ui->stacked_widget->setCurrentWidget(widget);
    emit current_widget_changed();
}

void Main_Window::make_button(const QString &name){
    QPushButton *pdf_list_push_button = new QPushButton();
    pdf_list_push_button->setFixedSize(150, 30);
    pdf_list_push_button->setText(name);

    connect(pdf_list_push_button, &QPushButton::clicked, this, [this, name]{
        QWidget *widget = ui->stacked_widget->findChild<QWidget*>(name);
        if(widget){
            // 위젯 변경 시, 기존 위젯의 오버레이 위젯 초기화
            QWidget *current_widget = ui->stacked_widget->currentWidget();
            qDebug() << widget << current_widget;
            if(current_widget && current_widget != widget){
                if(current_widget->layout()->indexOf(event_overlay_widget) != -1){
                    current_widget->layout()->removeWidget(event_overlay_widget);
                    event_overlay_widget->set_paint_mode(-1);
                    event_overlay_widget->setParent(nullptr);
                    event_overlay_widget->hide();
                }
            }
            ui->stacked_widget->setCurrentWidget(widget);
            emit current_widget_changed();
        }
    });

    pdf_list_layout->addWidget(pdf_list_push_button);
}

void Main_Window::page_line_edit_return_pressed(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &input_text){
    bool ok = false;
    int index = input_text.toInt(&ok) - 1;
    if(ok && index >= 0 && index <= total_page_index){
        pdf_viewer_widget->page_changed(index);
    }

    page_line_edit->setText(QString::number(pdf_viewer_widget->get_current_page_index() + 1));
}

void Main_Window::action_prev_page_triggered(){
    Pdf_Viewer_Widget *pdf_viewer_widget = get_current_pdf_viewer_widget();
    if(pdf_viewer_widget && current_page_index > 0){
        int prev_page_index = current_page_index - 1;
        pdf_viewer_widget->page_changed(prev_page_index);
    }
}

void Main_Window::action_next_page_triggered(){
    Pdf_Viewer_Widget *pdf_viewer_widget = get_current_pdf_viewer_widget();
    if(pdf_viewer_widget && current_page_index < total_page_index){
        int next_page_index = current_page_index + 1;
        pdf_viewer_widget->page_changed(next_page_index);
    }
}

void Main_Window::action_toggle_triggered(QWidget *widget){
    Pdf_Viewer_Widget *pdf_viewer_widget = widget->findChild<Pdf_Viewer_Widget*>();
    if(pdf_viewer_widget){
        QPdfView::PageMode current_page_mode = pdf_viewer_widget->get_current_page_mode();
        if(widget->layout()->indexOf(event_overlay_widget) != -1 && current_page_mode == QPdfView::PageMode::SinglePage){
            // 레이아웃으로부터 제거
            widget->layout()->removeWidget(event_overlay_widget);
            event_overlay_widget->set_paint_mode(-1);
            event_overlay_widget->setParent(nullptr);
            event_overlay_widget->hide();

            pdf_viewer_widget->set_page_mode(QPdfView::PageMode::MultiPage);
        }
        else if(widget->layout()->indexOf(event_overlay_widget) == -1){
            // 위젯 추가
            widget->layout()->addWidget(event_overlay_widget);
            event_overlay_widget->show();
            event_overlay_widget->raise();  // 위젯 위치를 최상위로 설정

            if(current_page_mode == QPdfView::PageMode::MultiPage){
               pdf_viewer_widget->set_page_mode(QPdfView::PageMode::SinglePage);
            }
        }
    }
}
