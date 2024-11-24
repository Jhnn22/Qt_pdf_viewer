#include "pdf_viewer_widget.h"
#include "./ui_pdf_viewer_widget.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QScrollBar>
#include <QMetaObject>
#include <QMetaMethod>
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

void Pdf_Viewer_Widget::set_page_mode(QPdfView::PageMode page_mode){
    pdf_view->setPageMode(page_mode);
}

QPdfView::PageMode Pdf_Viewer_Widget::get_current_page_mode(){
    return pdf_view->pageMode();
}

void Pdf_Viewer_Widget::set_connects(){
    // 스크롤 바 사용시 페이지 표시 업데이트
    connect(pdf_view->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](){
        if(using_tool_bar){
            return;
        }
        prev_page_index = current_page_index;
        current_page_index = pdf_page_navigator->currentPage();
        if(prev_page_index != current_page_index){
            prev_page_index = current_page_index;
            emit update_page_info();
        }
    });

    connect(pdf_view, &QPdfView::pageModeChanged, this, [this](QPdfView::PageMode changed_page_mode){
        if(changed_page_mode == QPdfView::PageMode::MultiPage){
            QMetaObject::invokeMethod(this, [this, changed_page_mode]{
                pdf_page_navigator->jump(current_page_index, {}, pdf_page_navigator->currentZoom());
            }, Qt::QueuedConnection);
        }
    });
}

void Pdf_Viewer_Widget::set_pdf_viewer(){
    // pdf에 표시할 문서를 설정
    pdf_view->setDocument(pdf_document);
    pdf_document->load(url.toLocalFile());

    // pdf 설정
    pdf_page_navigator = pdf_view->pageNavigator();
    current_page_index = pdf_page_navigator->currentPage();
    pdf_page_navigator->jump(current_page_index, {}, pdf_page_navigator->currentZoom());
    pdf_view->setDocumentMargins(QMargins(0, 0, 0, 0));
    pdf_view->setPageMode(QPdfView::PageMode::MultiPage);
    pdf_view->setZoomMode(QPdfView::ZoomMode::FitInView);
    this->layout()->addWidget(pdf_view);
}

