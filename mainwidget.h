#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QUrl>

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

private:
    Ui::MainWidget *ui;

    QFileDialog *m_fileDialog;              // 파일 탐색기
    QPdfView *m_pdfView;                    // pdf를 표시
    QPdfDocument *m_pdfDocument;            // pdf의 데이터를 제공
    QPdfPageNavigator *m_pdfPageNavigator;  // pdf의 페이지 탐색

private:
    void setConnects();
    void open(const QUrl &pdfLocation);
    void pageSelected(int page);
};
#endif // MAINWIDGET_H
