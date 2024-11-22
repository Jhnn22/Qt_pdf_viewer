#include "pdf_viewer_widget.h"
#include "./ui_pdf_viewer_widget.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QScrollBar>
#include <QTimer>

Pdf_Viewer_Widget::Pdf_Viewer_Widget(const QUrl &url, QWidget *parent)
    : url(url), QWidget(parent)
    , ui(new Ui::Pdf_Viewer_Widget)
    , pdf_view(new QPdfView(this)), pdf_document(new QPdfDocument(this))
    , using_tool_bar(false)
{
    ui->setupUi(this);

    set_pdf_viewer();
    set_connects();
}

Pdf_Viewer_Widget::~Pdf_Viewer_Widget()
{
    delete pdf_view;
    delete pdf_document;
    delete ui;
}

int Pdf_Viewer_Widget::get_current_page_index(){
    return pdf_page_navigator->currentPage();
}

int Pdf_Viewer_Widget::get_total_page_index(){
    return pdf_document->pageCount() - 1;
}

void Pdf_Viewer_Widget::page_changed(const int changed_page_index){
    using_tool_bar = true;

    pdf_page_navigator->jump(changed_page_index, {}, pdf_page_navigator->currentZoom());
    emit update_page_info();

    using_tool_bar = false;
}

qreal Pdf_Viewer_Widget::get_current_zoom_factor(){
    return pdf_view->zoomFactor();
}

void Pdf_Viewer_Widget::zoom_changed(const qreal changed_zoom_factor){
    QScrollBar *h = pdf_view->horizontalScrollBar();
    QScrollBar *v = pdf_view->verticalScrollBar();

    int h_value = h->value(); int h_max_value = h->maximum();
    int v_value = v->value(); int v_max_value = v->maximum();

    // 줌 설정
    pdf_view->setZoomFactor(changed_zoom_factor);
    emit update_zoom_info();

    // 스크롤 값 계산
    int new_h_max_value = h->maximum();
    int new_v_max_value = v->maximum();

    int new_h_value = (h_max_value == 0) ? 0 :
                          qBound(0, static_cast<int>(h_value * (static_cast<qreal>(new_h_max_value) / h_max_value)), new_h_max_value);

    int new_v_value = (v_max_value == 0) ? 0 :
                          qBound(0, static_cast<int>(v_value * (static_cast<qreal>(new_v_max_value) / v_max_value)), new_v_max_value);

    // 새로운 스크롤 값 적용
    h->setValue(new_h_value);
    v->setValue(new_v_value);
}

QPdfView::PageMode Pdf_Viewer_Widget::get_current_page_mode(){
    return pdf_view->pageMode();
}

void Pdf_Viewer_Widget::page_mode_changed(QPdfView::PageMode changed_page_mode){
    current_page_index = pdf_page_navigator->currentPage(); // 현재 페이지의 인덱스 저장

    pdf_view->setPageMode(changed_page_mode);
    // QTimer를 사용하여 약간의 지연 후 기존 페이지 위치로 이동
    // QTimer::singleShot(30, this, [this](){
    //     pdf_page_navigator->jump(current_page_index, {}, pdf_page_navigator->currentZoom());
    // });
}

void Pdf_Viewer_Widget::set_connects(){
    // 스크롤 바 사용시 페이지 표시 업데이트
    prev_page_index = pdf_page_navigator->currentPage();
    connect(pdf_view->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](){
        if(using_tool_bar){
            return;
        }

        current_page_index = pdf_page_navigator->currentPage();
        if(prev_page_index != current_page_index){
            prev_page_index = current_page_index;
            emit update_page_info();
        }
    });
}

void Pdf_Viewer_Widget::set_pdf_viewer(){
    // pdf에 표시할 문서를 설정
    pdf_view->setDocument(pdf_document);
    pdf_document->load(url.toLocalFile());

    // pdf 설정
    pdf_page_navigator = pdf_view->pageNavigator();
    pdf_page_navigator->jump(0, {}, pdf_page_navigator->currentZoom());
    pdf_view->setDocumentMargins(QMargins(0, 0, 0, 0));
    pdf_view->setPageMode(QPdfView::PageMode::MultiPage);
    this->layout()->addWidget(pdf_view);
}

