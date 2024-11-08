#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QUrl>

class PageSelector;
class ZoomSelector;

class QFileDialog;
class QPdfView;
class QPdfDocument;
class QPdfPageNavigator;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

signals:
    void set_PdfPageInfo(const int begin, const int end);

private:
    Ui::MainWidget *ui;

    PageSelector *m_pageSelector;
    ZoomSelector *m_zoomSelector;

    QFileDialog *m_fileDialog;              // 파일 탐색기
    QPdfView *m_pdfView;                    // pdf를 표시
    QPdfDocument *m_pdfDocument;            // pdf의 데이터를 제공
    QPdfPageNavigator *m_pdfPageNavigator;  // pdf의 페이지 탐색

private:
    int currentIndex;

private:
    void setConnects();
    void open(const QUrl &pdfLocation);
    void pageSelected(int page);

private slots:
    void load_pushButton_clicked();
};
#endif // MAINWIDGET_H
