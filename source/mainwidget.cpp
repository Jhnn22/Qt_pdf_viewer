#include "mainwidget.h"
#include "./ui_mainwidget.h"

#include "pageselector.h"
#include "zoomselector.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QPdfView>
#include <QPdfDocument>
#include <QPdfPageNavigator>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , m_pageSelector(new PageSelector), m_zoomSelector(new ZoomSelector)
    , m_fileDialog(nullptr), m_pdfView(new QPdfView(this)), m_pdfDocument(new QPdfDocument(this))
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(m_pdfView);

    m_pdfView->setDocument(m_pdfDocument);                  // pdf에 표시할 문서를 설정
    m_pdfView->setDocumentMargins(QMargins(0, 0, 0, 0));    // pdf 외부 여백 설정
    m_pdfView->setPageMode(QPdfView::PageMode::MultiPage);  // pdf를 여러 페이지로 표시, 스크롤 기능 지원

    setConnects();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::setConnects(){
    connect(ui->load_pushButton, &QPushButton::clicked, this, &MainWidget::load_pushButton_clicked);    // pdf 불러오기

    // 페이지 관리--------------------------------------------------------------
    // 페이지 이동
    m_pdfPageNavigator = m_pdfView->pageNavigator();
    connect(ui->back_pushButton, &QPushButton::clicked, this, [this](){
        currentIndex = m_pdfPageNavigator->currentPage();
        m_pageSelector->pageSelect_with_back_pushButton(currentIndex);
    });
    connect(ui->forward_pushButton, &QPushButton::clicked, this, [this](){
        currentIndex = m_pdfPageNavigator->currentPage();
        m_pageSelector->pageSelect_with_forward_pushButton(currentIndex);
    });
    connect(ui->page_lineEdit, &QLineEdit::returnPressed, this, [this](){
        QString inputText = ui->page_lineEdit->text();
        m_pageSelector->pageSelect(inputText);
    });
    // 페이지 적용 및 표시
    connect(m_pageSelector, &PageSelector::pageChanged, this, [this](const int pageIndex, const QString &text){
        if(pageIndex < 0){
            ui->page_lineEdit->setText(QString::number(m_pdfPageNavigator->currentPage() + 1)); // 잘못된 입력인 경우, 현재 페이지를 표시
        }
        else{
            m_pdfPageNavigator->jump(pageIndex, {}, m_pdfPageNavigator->currentZoom());
            ui->page_lineEdit->setText(text);
        }
    });

    // 화면 비율 관리--------------------------------------------------------------
    // 화면 비율 선택
    connect(ui->zoomOut_pushButton, &QPushButton::clicked, this, [this](){
        currentZoomFactor = m_pdfView->zoomFactor();
        m_zoomSelector->zoomSelect_with_zoomOut_pushButton(currentZoomFactor);
    });
    connect(ui->zoomIn_pushButton, &QPushButton::clicked, this, [this](){
        currentZoomFactor = m_pdfView->zoomFactor();
        m_zoomSelector->zoomSelect_with_zoomIn_pushButton(currentZoomFactor);
    });
    connect(ui->zoom_lineEdit, &QLineEdit::returnPressed, this, [this](){
        QString inputText = ui->zoom_lineEdit->text();
        m_zoomSelector->zoomSelect(inputText);
    });
    // 화면 비율 적용 및 표시
    connect(m_zoomSelector, &ZoomSelector::zoomChanged, this, [this](const qreal zoomFactor, const QString &text){
        m_pdfView->setZoomFactor(zoomFactor);
        ui->zoom_lineEdit->setText(text);

        qDebug() << m_pdfView->zoomFactor();    // 확인용 디버그 코드
    });
}

void MainWidget::open(const QUrl &pdfLocation){
    if(pdfLocation.isLocalFile()){
        m_pdfDocument->load(pdfLocation.toLocalFile());
        pageSelected(0);
        m_pageSelector->getPdfIndexInfo(0, m_pdfDocument->pageCount() - 1);

        // 초기 화면 설정
        ui->page_lineEdit->setText(QString::number(m_pdfPageNavigator->currentPage() + 1));
        ui->totalPage_label->setText(" / " + QString::number(m_pdfDocument->pageCount()));
        ui->zoom_lineEdit->setText(QString::number(m_pdfView->zoomFactor() * 100).append('%'));
    }
    else{
        qDebug() << "failed to open";
    }
}

void MainWidget::pageSelected(int page){
    m_pdfPageNavigator = m_pdfView->pageNavigator();
    m_pdfPageNavigator->jump(page, {}, m_pdfPageNavigator->currentZoom());    // pdf의 page페이지로 이동
}

void MainWidget::load_pushButton_clicked(){
    if(m_fileDialog == nullptr){
        m_fileDialog = new QFileDialog(this, tr("Pdf 불러오기"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));   // 기본 경로 : 다운로드
        m_fileDialog->setAcceptMode(QFileDialog::AcceptOpen);   // 열기 모드
        m_fileDialog->setMimeTypeFilters({"application/pdf"});  // pdf 문서만 표시
    }
    if(m_fileDialog->exec() == QDialog::Accepted){
        const QUrl toOpen = m_fileDialog->selectedUrls().constFirst();
        if(toOpen.isValid()){
            open(toOpen);
        }
    }
}
