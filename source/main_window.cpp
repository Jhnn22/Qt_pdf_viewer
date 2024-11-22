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
    , file_dialog(nullptr)
    , zoom_factors{0.25, 0.33, 0.50, 0.67, 0.75, 0.80, 0.90, 1.00,
                   1.10, 1.25, 1.50, 1.75, 2.00, 2.50, 3.00, 4.00, 5.00}
    , event_overlay_widget(new Event_Overlay_Widget(nullptr))
{
    ui->setupUi(this);

    file_list_layout = qobject_cast<QVBoxLayout*>(ui->widget->layout());
    if(file_list_layout){
        file_list_layout->setContentsMargins(0, 0, 0, 0);
        file_list_layout->setAlignment(Qt::AlignTop);
    }

    set_tool_bar();
    set_connects();
}

Main_Window::~Main_Window()
{
    delete ui;
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
    page_line_edit->setFixedSize(20, 20);
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

    // 줌
    action_zoom_out = new QAction(this);
    action_zoom_out->setText("-");
    action_zoom_out->setToolTip("확대");

    action_zoom_in = new QAction(this);
    action_zoom_in->setText("+");
    action_zoom_out->setToolTip("축소");

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
    ui->tool_bar->addAction(action_zoom_out);
    ui->tool_bar->addAction(action_zoom_in);
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
            total_page_index = pdf_viewer_widget->get_total_page_index();
            page_line_edit->setText(QString::number(current_page_index + 1));
            total_page_label->setText(QString::number(total_page_index + 1));

            current_zoom_factor = pdf_viewer_widget->get_current_zoom_factor();
        }
    });
    // 페이지
    connect(page_line_edit, &QLineEdit::returnPressed, this, [this](){
        Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
        if(pdf_viewer_widget){
            QString str = page_line_edit->text();
            page_line_edit_return_pressed(pdf_viewer_widget, str);
        }
    });
    connect(action_prev_page, &QAction::triggered, this, [this](){
        Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
        if(pdf_viewer_widget){
            action_prev_page_triggered(pdf_viewer_widget);
        }
    });
    connect(action_next_page, &QAction::triggered, this, [this](){
        Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
        if(pdf_viewer_widget){
            action_next_page_triggered(pdf_viewer_widget);
        }
    });
    // 줌
    connect(action_zoom_out, &QAction::triggered, this, [this](){
        Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
        if(pdf_viewer_widget){
            action_zoom_out_triggered(pdf_viewer_widget);
        }
    });
    connect(action_zoom_in, &QAction::triggered, this, [this](){
        Pdf_Viewer_Widget *pdf_viewer_widget = ui->stacked_widget->currentWidget()->findChild<Pdf_Viewer_Widget*>();
        if(pdf_viewer_widget){
            action_zoom_in_triggered(pdf_viewer_widget);
        }
    });
    // 페인트 이벤트
    connect(action_toggle, &QAction::triggered, this, [this](){
        QWidget *page = ui->stacked_widget->currentWidget();
        if(page){
            action_toggle_triggered(page);
        }
    });
    connect(action_pointing, &QAction::triggered, this, [this](){
        event_overlay_widget->set_paint_mode(POINTING);
    });
    connect(action_drawing, &QAction::triggered, this, [this](){
        event_overlay_widget->set_paint_mode(DRAWING);
    });

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

void Main_Window::page_line_edit_return_pressed(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &input_text){
    bool ok = false;
    int index = input_text.toInt(&ok) - 1;
    if(ok && index >= 0 && index <= total_page_index){
        pdf_viewer_widget->page_changed(index);
    }

    page_line_edit->setText(QString::number(pdf_viewer_widget->get_current_page_index() + 1));
}

void Main_Window::action_prev_page_triggered(Pdf_Viewer_Widget *pdf_viewer_widget){
    if(current_page_index > 0){
        int prev_page_index = current_page_index - 1;
        pdf_viewer_widget->page_changed(prev_page_index);
    }
}

void Main_Window::action_next_page_triggered(Pdf_Viewer_Widget *pdf_viewer_widget){
    if(current_page_index < total_page_index){
        int next_page_index = current_page_index + 1;
        pdf_viewer_widget->page_changed(next_page_index);
    }
}

void Main_Window::action_zoom_out_triggered(Pdf_Viewer_Widget *pdf_viewer_widget){
    current_zoom_factor_index = find_nearest_zoom_factor(current_zoom_factor);
    if(current_zoom_factor_index <= 0){
        pdf_viewer_widget->zoom_changed(zoom_factors.first());
        return;
    }
    pdf_viewer_widget->zoom_changed(zoom_factors[current_zoom_factor_index - 1]);
}

void Main_Window::action_zoom_in_triggered(Pdf_Viewer_Widget *pdf_viewer_widget){
    current_zoom_factor_index = find_nearest_zoom_factor(current_zoom_factor);
    if(current_zoom_factor_index >= zoom_factors.size() - 1){
        pdf_viewer_widget->zoom_changed(zoom_factors.last());
        return;
    }
    pdf_viewer_widget->zoom_changed(zoom_factors[current_zoom_factor_index + 1]);
}

int Main_Window::find_nearest_zoom_factor(const qreal current_zoom_factor){                     // 정수 값으로 변환
    auto it = std::lower_bound(zoom_factors.begin(), zoom_factors.end(), current_zoom_factor);  // 가장 가까운 값 확인
    if(it == zoom_factors.end()){
        return zoom_factors.size() - 1;
    }
    int index = it - zoom_factors.begin();

    return index;
}

void Main_Window::action_toggle_triggered(QWidget *page){
    Pdf_Viewer_Widget *pdf_viewer_widget = page->findChild<Pdf_Viewer_Widget*>();
    if(pdf_viewer_widget){
        QPdfView::PageMode current_page_mode = pdf_viewer_widget->get_current_page_mode();
        if(current_page_mode == QPdfView::PageMode::SinglePage && page->layout()->indexOf(event_overlay_widget) > -1){
            pdf_viewer_widget->page_mode_changed(QPdfView::PageMode::MultiPage);
            // 레이아웃으로부터 제거
            page->layout()->removeWidget(event_overlay_widget);
            event_overlay_widget->set_paint_mode(-1);
            event_overlay_widget->setParent(nullptr);
            event_overlay_widget->hide();

        }
        else if(current_page_mode == QPdfView::PageMode::MultiPage && page->layout()->indexOf(event_overlay_widget) == -1){
            pdf_viewer_widget->page_mode_changed(QPdfView::PageMode::SinglePage);
            // 위젯 추가
            page->layout()->addWidget(event_overlay_widget);
            event_overlay_widget->show();
            event_overlay_widget->raise();  // 위젯 위치를 최상위로 설정
        }
    }
}

void Main_Window::open_file(const QUrl &url){
    if(url.isLocalFile()){
        Pdf_Viewer_Widget *pdf_viewer_widget = new Pdf_Viewer_Widget(url, this);
        connect(pdf_viewer_widget, &Pdf_Viewer_Widget::update_page_info, this, [this, pdf_viewer_widget](){
            current_page_index = pdf_viewer_widget->get_current_page_index();
            qDebug() << "현재 페이지:" << current_page_index + 1;
            page_line_edit->setText(QString::number(current_page_index + 1));
        });
        connect(pdf_viewer_widget, &Pdf_Viewer_Widget::update_zoom_info, this, [this, pdf_viewer_widget](){
            current_zoom_factor = pdf_viewer_widget->get_current_zoom_factor();
            qDebug() << "현재 화면 비율:" << current_zoom_factor;
        });

        make_page(pdf_viewer_widget, url.fileName());
        make_button(url.fileName());
    }
    else{
        qDebug() << "failed to open";
    }
}

void Main_Window::make_page(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &name){
    QWidget *page = new QWidget(ui->stacked_widget);
    page->setObjectName(name);
    QStackedLayout *stacked_layout = new QStackedLayout(page);
    stacked_layout->setStackingMode(QStackedLayout::StackAll);
    stacked_layout->setContentsMargins(0, 0, 0, 0);
    stacked_layout->addWidget(pdf_viewer_widget);
    page->setLayout(stacked_layout);

    ui->stacked_widget->layout()->addWidget(page);
}

void Main_Window::make_button(const QString &name){
    QPushButton *file_list_push_button = new QPushButton();
    file_list_push_button->setFixedSize(150, 30);
    file_list_push_button->setText(name);

    file_list_layout->addWidget(file_list_push_button);

    connect(file_list_push_button, &QPushButton::clicked, this, [this, name]{
        QWidget *page = ui->stacked_widget->findChild<QWidget*>(name);
        if(page){
            ui->stacked_widget->setCurrentWidget(page);
            emit current_widget_changed();
        }
    });
}
