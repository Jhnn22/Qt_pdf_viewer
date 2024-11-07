#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QUrl>

class QPdfView;
class QPdfDocument;
class QFileDialog;

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

    QPdfView *m_pdfView;
    QPdfDocument *m_pdfDocument;
    QFileDialog *m_fileDialog = nullptr;


private:
    void setConnects();
    void open(const QUrl &pdfLocation);
    void pageSelected(int page);
};
#endif // MAINWIDGET_H
