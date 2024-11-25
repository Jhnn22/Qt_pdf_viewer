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
    , full_screen_widget(new QWidget(nullptr))
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
    total_page_label->setFixedSize(30, 30);
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

    action_full_screen = new QAction(this);
    action_full_screen->setText("F");
    action_full_screen->setToolTip("전체 화면");
    action_full_screen->setShortcut(Qt::Key_F5);

    // 페인트 이벤트
    action_pointing = new QAction(this);
    action_pointing->setText("P");
    action_pointing->setToolTip("포인터");
    action_pointing->setShortcut(Qt::Key_P);

    action_drawing = new QAction(this);
    action_drawing->setText("D");
    action_drawing->setToolTip("드로잉");
    action_drawing->setShortcut(Qt::Key_D);

    // 순서에 맞게 툴 바에 추가
    ui->tool_bar->addWidget(spacer);
    ui->tool_bar->addWidget(widget);
    ui->tool_bar->addAction(action_prev_page);
    ui->tool_bar->addAction(action_next_page);
    ui->tool_bar->addAction(action_full_screen);
    ui->tool_bar->addAction(action_pointing);
    ui->tool_bar->addAction(action_drawing);
}

void Main_Window::set_connects(){
    // 파일 목록
    connect(ui->load_push_button, &QPushButton::clicked, this, &Main_Window::load_push_button_clicked);

    // 툴바
    // pdf 로드 및 변경 시 툴바 업데이트
    connect(this, &Main_Window::current_widget_changed, this, [this](){
        Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
        if(pdf_viewer_widget){
            current_page_index = pdf_viewer_widget->get_current_page_index();
            page_line_edit->setText(QString::number(current_page_index + 1));
            total_page_index = pdf_viewer_widget->get_total_page_index();
            total_page_label->setText(QString::number(total_page_index + 1));

            qDebug() << "추가/변경된 pdf의 현재 페이지:" << current_page_index + 1;
        }
    });
    // 페이지
    connect(page_line_edit, &QLineEdit::returnPressed, this, [this](){
        QString input_text = page_line_edit->text();
        page_line_edit_return_pressed(input_text);
    });
    connect(action_prev_page, &QAction::triggered, this, &Main_Window::action_prev_page_triggered);
    connect(action_next_page, &QAction::triggered, this, &Main_Window::action_next_page_triggered);
    connect(action_full_screen, &QAction::triggered, this, &Main_Window::action_full_screen_triggered);
    connect(event_overlay_widget, &Event_Overlay_Widget::screen_restore, this, &Main_Window::screen_restore);

    connect(action_pointing, &QAction::triggered, this, &Main_Window::action_pointing_triggered);
    connect(action_drawing, &QAction::triggered, this, &Main_Window::action_drawing_triggered);
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
        make_widget(pdf_viewer_widget, url.fileName());
        make_button(url.fileName());

        connect(pdf_viewer_widget, &Pdf_Viewer_Widget::update_page_line_edit, this, [this, pdf_viewer_widget](){
            current_page_index = pdf_viewer_widget->get_current_page_index();
            qDebug() << "기존 pdf의 현재 페이지:" << current_page_index + 1;
            page_line_edit->setText(QString::number(current_page_index + 1));
        });
    }
    else{
        qDebug() << "failed to open";
        return;
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
    pdf_list_layout->addWidget(pdf_list_push_button);

    connect(pdf_list_push_button, &QPushButton::clicked, this, [this, name]{
        QWidget *named_widget = ui->stacked_widget->findChild<QWidget*>(name);
        if(named_widget){
            QWidget *current_widget = ui->stacked_widget->currentWidget();
            if(current_widget && current_widget != named_widget){
                ui->stacked_widget->setCurrentWidget(named_widget);

                emit current_widget_changed();
            }
        }
    });
}

void Main_Window::page_line_edit_return_pressed(const QString &input_text){
    Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
    if(pdf_viewer_widget){
        bool ok = false;
        int index = input_text.toInt(&ok) - 1;
        if(ok && index >= 0 && index <= total_page_index){
            pdf_viewer_widget->page_changed(index);
        }
        page_line_edit->setText(QString::number(pdf_viewer_widget->get_current_page_index() + 1));
    }
}

void Main_Window::action_prev_page_triggered(){
    Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
    if(pdf_viewer_widget && current_page_index > 0){
        int prev_page_index = current_page_index - 1;
        pdf_viewer_widget->page_changed(prev_page_index);
    }
}

void Main_Window::action_next_page_triggered(){
    Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
    if(pdf_viewer_widget && current_page_index < total_page_index){
        int next_page_index = current_page_index + 1;
        pdf_viewer_widget->page_changed(next_page_index);
    }
}

void Main_Window::action_full_screen_triggered(){
    full_screen_widget = ui->stacked_widget->currentWidget();
    if(full_screen_widget){
        pdf_viewer_widget = full_screen_widget->findChild<Pdf_Viewer_Widget*>();
        if(pdf_viewer_widget){
            pdf_viewer_widget->set_page_mode(QPdfView::PageMode::SinglePage);
        }
        else{
            qDebug() << "no pdf document is open";
            return;
        }

        full_screen_widget->layout()->addWidget(event_overlay_widget);
        event_overlay_widget->show();
        event_overlay_widget->raise();

        ui->stacked_widget->removeWidget(full_screen_widget);
        ui->stacked_widget->setCurrentIndex(0);
        this->hide();
        full_screen_widget->setParent(nullptr);
        full_screen_widget->showFullScreen();
        event_overlay_widget->setFocus();
    }
}

void Main_Window::screen_restore(){
    pdf_viewer_widget->set_page_mode(QPdfView::PageMode::MultiPage);
    full_screen_widget->layout()->removeWidget(event_overlay_widget);
    event_overlay_widget->setParent(nullptr);
    event_overlay_widget->set_paint_mode(-1);
    event_overlay_widget->hide();

    this->show();
    ui->stacked_widget->addWidget(full_screen_widget);
    ui->stacked_widget->setCurrentWidget(full_screen_widget);
    full_screen_widget->setFocus();

    emit current_widget_changed();
}

void Main_Window::action_pointing_triggered(){
    event_overlay_widget->set_paint_mode(POINTING);
}

void Main_Window::action_drawing_triggered(){
    event_overlay_widget->set_paint_mode(DRAWING);
}
