#include "mainwidget.h"
#include "./ui_mainwidget.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QPdfView>
#include <QPdfDocument>
#include <QPdfPageNavigator>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
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
    // 불러오기
    connect(ui->load_pushButton, &QPushButton::clicked, this, [this](){
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
    });

    m_pdfPageNavigator = m_pdfView->pageNavigator();
    // 이전 페이지
    connect(ui->back_pushButton, &QPushButton::clicked, this, [this](){
        // 첫 페이지에서는 이전 페이지로 이동 x
        if(m_pdfPageNavigator->currentPage() > 0){
            m_pdfPageNavigator->jump(m_pdfPageNavigator->currentPage() - 1, {}, m_pdfPageNavigator->currentZoom());
        }
    });
    // 다음 페이지
    connect(ui->forward_pushButton, &QPushButton::clicked, this, [this](){
        // 마지막 페이지에서는 다음 페이지로 이동 x
        if(m_pdfPageNavigator->currentPage() < m_pdfDocument->pageCount() - 1){
            m_pdfPageNavigator->jump(m_pdfPageNavigator->currentPage() + 1, {}, m_pdfPageNavigator->currentZoom());
        }
    });
    // 페이지 선택
    connect(ui->pageInput_lineEdit, &QLineEdit::returnPressed, this, [this](){
        QString page_str = ui->pageInput_lineEdit->text();
        // 페이지 번호 정수 변환 및 처리
        bool ok;
        int page_int = page_str.toInt(&ok);
        if(ok && page_int >= 1 && page_int <= m_pdfDocument->pageCount()){
            pageSelected(page_int - 1);
        }
        else{
            ui->pageInput_lineEdit->setText(QString::number(m_pdfPageNavigator->currentPage() + 1));    // 잘못된 입력인 경우, 현재 페이지를 표시
        }
    });
}

void MainWidget::open(const QUrl &pdfLocation){
    if(pdfLocation.isLocalFile()){
        m_pdfDocument->load(pdfLocation.toLocalFile());
        pageSelected(0);
    }
    else{
        qDebug() << "failed to open";
    }
}

void MainWidget::pageSelected(int page){
    m_pdfPageNavigator = m_pdfView->pageNavigator();
    m_pdfPageNavigator->jump(page, {}, m_pdfPageNavigator->currentZoom());    // pdf의 page페이지로 이동
}
