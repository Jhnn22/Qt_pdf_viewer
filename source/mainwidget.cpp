#include "mainwidget.h"
#include "./ui_mainwidget.h"

#include "pageselector.h"
#include "zoomselector.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QPdfView>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QScrollBar>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , pageSelector(new PageSelector), zoomSelector(new ZoomSelector)
    , fileDialog(nullptr), pdfView(new QPdfView(this)), pdfDocument(new QPdfDocument(this))
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(pdfView);

    pdfView->setDocument(pdfDocument);                      // pdf에 표시할 문서를 설정
    pdfView->setDocumentMargins(QMargins(0, 0, 0, 0));      // pdf 외부 여백 설정
    pdfView->setPageMode(QPdfView::PageMode::MultiPage);    // pdf를 여러 페이지로 표시, 스크롤 기능 지원

    setConnects();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::setConnects(){
    connect(ui->loadPushButton, &QPushButton::clicked, this, &MainWidget::loadPushButtonClicked);    // pdf 불러오기

    // 페이지 관리--------------------------------------------------------------
    // 페이지 이동
    pdfPageNavigator = pdfView->pageNavigator();
    connect(ui->backPushButton, &QPushButton::clicked, this, [this](){
        currentIndex = pdfPageNavigator->currentPage();
        pageSelector->pageSelectWithBackPushButton(currentIndex);
    });
    connect(ui->forwardPushButton, &QPushButton::clicked, this, [this](){
        currentIndex = pdfPageNavigator->currentPage();
        pageSelector->pageSelectWithForwardPushButton(currentIndex);
    });
    connect(ui->pageLineEdit, &QLineEdit::returnPressed, this, [this](){
        QString inputText = ui->pageLineEdit->text();
        pageSelector->pageSelect(inputText);
    });
    // 페이지 적용 및 표시
    connect(pageSelector, &PageSelector::pageChanged, this, [this](const int pageIndex, const QString &text){
        if(pageIndex < 0){
            ui->pageLineEdit->setText(QString::number(pdfPageNavigator->currentPage() + 1)); // 잘못된 입력인 경우, 현재 페이지를 표시
        }
        else{
            pdfPageNavigator->jump(pageIndex, {}, pdfPageNavigator->currentZoom());
            ui->pageLineEdit->setText(text);
        }
    });
    // 스크롤 바 사용시 페이지 표시 업데이트
    prevIndex = pdfPageNavigator->currentPage();
    connect(pdfView->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](){
        currentIndex = pdfPageNavigator->currentPage();
        if(prevIndex != currentIndex){
            prevIndex = currentIndex;
            ui->pageLineEdit->setText(QString::number(currentIndex + 1));
        }
    });

    // 화면 비율 관리--------------------------------------------------------------
    // 화면 비율 선택
    connect(ui->zoomOutPushButton, &QPushButton::clicked, this, [this](){
        currentZoomFactor = pdfView->zoomFactor();
        zoomSelector->zoomSelectWithZoomOutPushButton(currentZoomFactor);
    });
    connect(ui->zoomInPushButton, &QPushButton::clicked, this, [this](){
        currentZoomFactor = pdfView->zoomFactor();
        zoomSelector->zoomSelectWithZoomInPushButton(currentZoomFactor);
    });
    connect(ui->zoomLineEdit, &QLineEdit::returnPressed, this, [this](){
        QString inputText = ui->zoomLineEdit->text();
        zoomSelector->zoomSelect(inputText);
    });
    // 화면 비율 적용 및 표시
    connect(zoomSelector, &ZoomSelector::zoomChanged, this, [this](const qreal zoomFactor, const QString &text){
        pdfView->setZoomFactor(zoomFactor);
        ui->zoomLineEdit->setText(text);

        qDebug() << pdfView->zoomFactor();    // 확인용 디버그 코드
    });
}

void MainWidget::open(const QUrl &pdfLocation){
    if(pdfLocation.isLocalFile()){
        pdfDocument->load(pdfLocation.toLocalFile());
        pageSelected(0);
        pageSelector->getPdfIndexInfo(0, pdfDocument->pageCount() - 1);

        // 초기 화면 설정
        ui->pageLineEdit->setText(QString::number(pdfPageNavigator->currentPage() + 1));
        ui->totalPageLabel->setText(" / " + QString::number(pdfDocument->pageCount()));
        ui->zoomLineEdit->setText(QString::number(pdfView->zoomFactor() * 100).append('%'));
    }
    else{
        qDebug() << "failed to open";
    }
}

void MainWidget::pageSelected(int page){
    pdfPageNavigator = pdfView->pageNavigator();
    pdfPageNavigator->jump(page, {}, pdfPageNavigator->currentZoom());    // pdf의 page페이지로 이동
}

void MainWidget::loadPushButtonClicked(){
    if(fileDialog == nullptr){
        fileDialog = new QFileDialog(this, tr("Pdf 불러오기"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));   // 기본 경로 : 다운로드
        fileDialog->setAcceptMode(QFileDialog::AcceptOpen);   // 열기 모드
        fileDialog->setMimeTypeFilters({"application/pdf"});  // pdf 문서만 표시
    }
    if(fileDialog->exec() == QDialog::Accepted){
        const QUrl toOpen = fileDialog->selectedUrls().constFirst();
        if(toOpen.isValid()){
            open(toOpen);
        }
    }
}
