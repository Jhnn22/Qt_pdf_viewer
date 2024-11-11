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
    void setPdfPageInfo(const int begin, const int end);

private:
    Ui::MainWidget *ui;

    PageSelector *pageSelector;
    ZoomSelector *zoomSelector;

    QFileDialog *fileDialog;              // 파일 탐색기
    QPdfView *pdfView;                    // pdf를 표시
    QPdfDocument *pdfDocument;            // pdf의 데이터를 제공
    QPdfPageNavigator *pdfPageNavigator;  // pdf의 페이지 탐색

private:
    int currentIndex;
    qreal currentZoomFactor;

private:
    void setConnects();
    void open(const QUrl &pdfLocation);
    void pageSelected(int page);

private slots:
    void loadPushButtonClicked();
};
#endif // MAINWIDGET_H
