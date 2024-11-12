#include "pdf_viewer_widget.h"
#include "./ui_pdf_viewer_widget.h"

#include "page_selector.h"
#include "zoom_selector.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QPdfView>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QScrollBar>
#include <QMouseEvent>

Pdf_Viewer_Widget::Pdf_Viewer_Widget(const QUrl &pdf_location, QWidget *parent)
    : pdf_location(pdf_location), QWidget(parent)
    , ui(new Ui::Pdf_Viewer_Widget)
    , page_selector(new Page_Selector), zoom_selector(new Zoom_Selector)
    , pdf_view(new QPdfView(this)), pdf_document(new QPdfDocument(this))
    , is_dragging(false)
{
    ui->setupUi(this);
    ui->v_layout->addWidget(pdf_view);

    // pdf에 표시할 문서를 설정
    pdf_view->setDocument(pdf_document);
    pdf_document->load(pdf_location.toLocalFile());

    // pdf 설정
    pdf_page_navigator = pdf_view->pageNavigator();
    pdf_page_navigator->jump(0, {}, pdf_page_navigator->currentZoom());
    page_selector->get_page_index_info(0, pdf_document->pageCount() - 1);
    pdf_view->setDocumentMargins(QMargins(0, 0, 0, 0));
    pdf_view->setPageMode(QPdfView::PageMode::MultiPage);
    pdf_view->viewport()->installEventFilter(this); // 이벤트 설치

    // pdf 초기 정보 표시
    ui->page_line_edit->setText(QString::number(pdf_page_navigator->currentPage() + 1));
    ui->total_page_label->setText(" / " + QString::number(pdf_document->pageCount()));
    ui->zoom_line_edit->setText(QString::number(pdf_view->zoomFactor() * 100).append('%'));

    set_connects();


}

Pdf_Viewer_Widget::~Pdf_Viewer_Widget()
{
    delete ui;
}

void Pdf_Viewer_Widget::set_connects(){
    // 페이지 관리--------------------------------------------------------------
    // 페이지 이동
    pdf_page_navigator = pdf_view->pageNavigator();
    connect(ui->prev_push_button, &QPushButton::clicked, this, [this](){
        current_page_index = pdf_page_navigator->currentPage();
        page_selector->page_select_with_prev_push_button(current_page_index);
    });
    connect(ui->next_push_button, &QPushButton::clicked, this, [this](){
        current_page_index = pdf_page_navigator->currentPage();
        page_selector->page_select_with_next_push_button(current_page_index);
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
    });
}

bool Pdf_Viewer_Widget::eventFilter(QObject *watched, QEvent *event){
    // viewport에서 발생한 이벤트만 처리
    if(watched != pdf_view->viewport()){
        return QWidget::eventFilter(watched, event);
    }

    // 마우스 이벤트만 처리
    if(event->type() != QEvent::MouseButtonPress && event->type() != QEvent::MouseMove && event->type() != QEvent::MouseButtonRelease){
        return QWidget::eventFilter(watched, event);
    }

    QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

    switch(event->type()){
    case QEvent::MouseButtonPress:
        if(mouse_event->button() == Qt::LeftButton){
            is_dragging = true;
            start_mouse_position = mouse_event->pos();
            qDebug() << "press position: " << start_mouse_position;
        }
        break;
    case QEvent::MouseMove:
        if(is_dragging){
            // QPoint position_diff = mouse_event->pos() - start_mouse_position;
            // start_mouse_position = mouse_event->pos();

            qDebug() << "current position: " << mouse_event->pos();

        }
        break;
    case QEvent::MouseButtonRelease:
        if(mouse_event->button() == Qt::LeftButton){
            is_dragging = false;
            qDebug() << "release position: " << mouse_event->pos();
        }
        break;
    }


    return QWidget::eventFilter(watched, event);
}
