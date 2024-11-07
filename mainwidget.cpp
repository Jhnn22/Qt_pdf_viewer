#include "mainwidget.h"
#include "./ui_mainwidget.h"
#include <QPdfView>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QFileDialog>
#include <QStandardPaths>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , m_pdfView(new QPdfView(this)), m_pdfDocument(new QPdfDocument(this))
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(m_pdfView);

    m_pdfView->setDocument(m_pdfDocument);
    m_pdfView->setDocumentMargins(QMargins(0, 0, 0, 0));
    m_pdfView->setPageMode(QPdfView::PageMode::MultiPage);

    setConnects();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::setConnects(){
    connect(ui->load_pushButton, &QPushButton::clicked, this, [this](){
        if(m_fileDialog == nullptr){
            m_fileDialog = new QFileDialog(this, tr("Pdf 불러오기"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
            m_fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
            m_fileDialog->setMimeTypeFilters({"application/pdf"});
        }
        if(m_fileDialog->exec() == QDialog::Accepted){
            const QUrl toOpen = m_fileDialog->selectedUrls().constFirst();
            if(toOpen.isValid()){
                open(toOpen);
                qDebug() << "open";
            }
        }

    });
}

void MainWidget::open(const QUrl &pdfLocation){
    if(pdfLocation.isLocalFile()){
        m_pdfDocument->load(pdfLocation.toLocalFile());
        pageSelected(0);
    }
    else{
        qDebug() << "open error";
    }
}

void MainWidget::pageSelected(int page){
    auto nav = m_pdfView->pageNavigator();
    nav->jump(page, {}, nav->currentZoom());
}
