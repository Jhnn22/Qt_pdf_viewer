#include "pdf_viewer_widget.h"
#include "./ui_pdf_viewer_widget.h"

#include "page_selector.h"
#include "zoom_selector.h"
#include "event_overlay_widget.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QScrollBar>
#include <QStackedLayout>
#include <QTimer>

Pdf_Viewer_Widget::Pdf_Viewer_Widget(const QUrl &pdf_location, QWidget *parent)
    : pdf_location(pdf_location), QWidget(parent)
    , ui(new Ui::Pdf_Viewer_Widget)
    , page_selector(new Page_Selector), zoom_selector(new Zoom_Selector)
    , event_overlay_widget(new Event_Overlay_Widget(nullptr))
    , pdf_view(new QPdfView(this)), pdf_document(new QPdfDocument(this))
{
    ui->setupUi(this);

    set_connects();
    set_pdf_viewer();
    set_stacked_layout();
}

Pdf_Viewer_Widget::~Pdf_Viewer_Widget()
{
    delete ui;
}

void Pdf_Viewer_Widget::set_connects(){
    // 페이지 관리--------------------------------------------------------------
    // 페이지 이동
    pdf_page_navigator = pdf_view->pageNavigator();
    connect(ui->prev_page_push_button, &QPushButton::clicked, this, [this](){
        current_page_index = pdf_page_navigator->currentPage();
        page_selector->page_select_with_prev_page_push_button(current_page_index);
    });
    connect(ui->next_page_push_button, &QPushButton::clicked, this, [this](){
        current_page_index = pdf_page_navigator->currentPage();
        page_selector->page_select_with_next_page_push_button(current_page_index);
    });
    connect(ui->page_line_edit, &QLineEdit::returnPressed, this, [this](){
        QString input_text = ui->page_line_edit->text();
        page_selector->page_select_with_page_line_edit(input_text);
    });
    // 페이지 적용 및 표시
    connect(page_selector, &Page_Selector::page_changed, this, [this](const int changed_page_index, const QString &changed_text){
        if(changed_page_index < 0){
            ui->page_line_edit->setText(QString::number(pdf_page_navigator->currentPage() + 1)); // 잘못된 입력인 경우, 현재 페이지를 표시
        }
        else{
            pdf_page_navigator->jump(changed_page_index, {}, pdf_page_navigator->currentZoom());
            ui->page_line_edit->setText(changed_text);
        }
        // event_overlay_widget이 겹쳐진 상태에서의 포커스 설정
        if(event_overlay_widget->parent() == this){
            event_overlay_widget->setFocus();
        }

    });
    // 스크롤 바 사용시 페이지 표시 업데이트
    prev_page_index = pdf_page_navigator->currentPage();
    connect(pdf_view->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](){
        current_page_index = pdf_page_navigator->currentPage();
        if(prev_page_index != current_page_index){
            prev_page_index = current_page_index;
            ui->page_line_edit->setText(QString::number(current_page_index + 1));
        }
    });

    // 화면 비율 관리--------------------------------------------------------------
    // 화면 비율 선택
    connect(ui->zoom_out_push_button, &QPushButton::clicked, this, [this](){
        current_zoom_factor = pdf_view->zoomFactor();
        zoom_selector->zoom_select_with_zoom_out_push_button(current_zoom_factor);
    });
    connect(ui->zoom_in_push_button, &QPushButton::clicked, this, [this](){
        current_zoom_factor = pdf_view->zoomFactor();
        zoom_selector->zoom_select_with_zoom_in_push_button(current_zoom_factor);
    });
    connect(ui->zoom_line_edit, &QLineEdit::returnPressed, this, [this](){
        QString input_text = ui->zoom_line_edit->text();
        zoom_selector->zoom_select_with_zoom_line_edit(input_text);
    });
    // 화면 비율 적용 및 표시
    connect(zoom_selector, &Zoom_Selector::zoom_changed, this, [this](const qreal changed_zoom_factor, const QString &changed_text){
        pdf_view->setZoomFactor(changed_zoom_factor);
        ui->zoom_line_edit->setText(changed_text);
        // event_overlay_widget이 겹쳐진 상태에서의 포커스 설정
        if(event_overlay_widget->parent() == this){
            event_overlay_widget->setFocus();
        }
    });

    // 이벤트 처리 위젯 관리--------------------------------------------------------------
    connect(ui->pointing_push_button, &QPushButton::clicked, this, [this](){
        event_overlay_widget->set_paint_mode(POINTING);
    });
    connect(ui->drawing_push_button, &QPushButton::clicked, this, [this](){
        event_overlay_widget->set_paint_mode(DRAWING);
    });
    connect(ui->toggle_push_button, &QPushButton::clicked, this, [this](){
        if(pdf_view->pageMode() == QPdfView::PageMode::SinglePage){
            set_page_mode(QPdfView::PageMode::MultiPage, false);
            emit toggle_push_button_clicked(MULTI_PAGE);

        }
        else{
            set_page_mode(QPdfView::PageMode::SinglePage, true);
            emit toggle_push_button_clicked(SINGLE_PAGE);
        }
        toggle_event_overlay_widget();
    });
}

void Pdf_Viewer_Widget::set_pdf_viewer(){
    // pdf에 표시할 문서를 설정
    pdf_view->setDocument(pdf_document);
    pdf_document->load(pdf_location.toLocalFile());

    // pdf 설정
    pdf_page_navigator = pdf_view->pageNavigator();
    pdf_page_navigator->jump(0, {}, pdf_page_navigator->currentZoom());
    page_selector->set_page_index_info(0, pdf_document->pageCount() - 1);
    pdf_view->setDocumentMargins(QMargins(0, 0, 0, 0));
    pdf_view->setPageMode(QPdfView::PageMode::MultiPage);
    pdf_view->viewport()->installEventFilter(this); // 이벤트 설치

    // pdf 초기 정보 표시
    ui->page_line_edit->setText(QString::number(pdf_page_navigator->currentPage() + 1));
    ui->total_page_label->setText(" / " + QString::number(pdf_document->pageCount()));
    ui->zoom_line_edit->setText(QString::number(pdf_view->zoomFactor() * 100).append('%'));
}

void Pdf_Viewer_Widget::set_stacked_layout(){
    stacked_layout = new QStackedLayout;
    stacked_layout->setStackingMode(QStackedLayout::StackAll);
    stacked_layout->addWidget(pdf_view);

    ui->v_layout->addLayout(stacked_layout);
}

void Pdf_Viewer_Widget::set_page_mode(QPdfView::PageMode current_page_mode, bool enabled){
    current_page_index = pdf_page_navigator->currentPage(); // 현재 페이지의 인덱스 저장
    pdf_view->setPageMode(current_page_mode);

    // QTimer를 사용하여 약간의 지연 후 기존 페이지 위치로 이동
    QTimer::singleShot(1, this, [this](){
        pdf_page_navigator->jump(current_page_index, {}, pdf_page_navigator->currentZoom());
    });

    ui->pointing_push_button->setEnabled(enabled);
    ui->drawing_push_button->setEnabled(enabled);
}

void Pdf_Viewer_Widget::toggle_event_overlay_widget(){
    if(stacked_layout->indexOf(event_overlay_widget) == -1){
        // 위젯 추가
        stacked_layout->addWidget(event_overlay_widget);
        event_overlay_widget->show();
        event_overlay_widget->raise();  // 위젯 위치를 최상위로 설정
    }
    else if(stacked_layout->indexOf(event_overlay_widget) != -1){
        // 저장된 라인 초기화
        event_overlay_widget->clear_total_lines();
        // 레이아웃으로부터 제거
        stacked_layout->removeWidget(event_overlay_widget);
        event_overlay_widget->setParent(nullptr);
        event_overlay_widget->hide();
    }
}
