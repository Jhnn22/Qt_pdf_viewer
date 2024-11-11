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

Pdf_Viewer_Widget::Pdf_Viewer_Widget(const QUrl &pdf_location, QWidget *parent)
    : pdf_location(pdf_location), QWidget(parent)
    , ui(new Ui::Pdf_Viewer_Widget)
    , page_selector(new Page_Selector), zoom_selector(new Zoom_Selector)
    , pdf_view(new QPdfView(this)), pdf_document(new QPdfDocument(this))
{
    ui->setupUi(this);
    ui->v_layout->addWidget(pdf_view);

    // pdf에 표시할 문서를 설정
    pdf_view->setDocument(pdf_document);
    pdf_document->load(pdf_location.toLocalFile());

    // pdf 초기 설정
    pdf_page_navigator = pdf_view->pageNavigator();
    pdf_page_navigator->jump(0, {}, pdf_page_navigator->currentZoom());
    page_selector->get_page_index_info(0, pdf_document->pageCount() - 1);
    pdf_view->setDocumentMargins(QMargins(0, 0, 0, 0));
    pdf_view->setPageMode(QPdfView::PageMode::MultiPage);

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
    connect(ui->back_push_button, &QPushButton::clicked, this, [this](){
        current_index = pdf_page_navigator->currentPage();
        page_selector->page_select_with_back_push_button(current_index);
    });
    connect(ui->forward_push_button, &QPushButton::clicked, this, [this](){
        current_index = pdf_page_navigator->currentPage();
        page_selector->page_select_with_forward_push_button(current_index);
    });
    connect(ui->page_line_edit, &QLineEdit::returnPressed, this, [this](){
        QString inputText = ui->page_line_edit->text();
        page_selector->page_select(inputText);
    });
    // 페이지 적용 및 표시
    connect(page_selector, &Page_Selector::page_changed, this, [this](const int page_index, const QString &text){
        if(page_index < 0){
            ui->page_line_edit->setText(QString::number(pdf_page_navigator->currentPage() + 1)); // 잘못된 입력인 경우, 현재 페이지를 표시
        }
        else{
            pdf_page_navigator->jump(page_index, {}, pdf_page_navigator->currentZoom());
            ui->page_line_edit->setText(text);
        }
    });
    // 스크롤 바 사용시 페이지 표시 업데이트
    prev_index = pdf_page_navigator->currentPage();
    connect(pdf_view->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](){
        current_index = pdf_page_navigator->currentPage();
        if(prev_index != current_index){
            prev_index = current_index;
            ui->page_line_edit->setText(QString::number(current_index + 1));
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
        zoom_selector->zoom_select(input_text);
    });
    // 화면 비율 적용 및 표시
    connect(zoom_selector, &Zoom_Selector::zoom_changed, this, [this](const qreal zoom_factor, const QString &text){
        pdf_view->setZoomFactor(zoom_factor);
        ui->zoom_line_edit->setText(text);

        qDebug() << pdf_view->zoomFactor();    // 확인용 디버그 코드
    });
}

