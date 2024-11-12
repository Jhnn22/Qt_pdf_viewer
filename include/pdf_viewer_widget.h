#ifndef PDF_VIEWER_WIDGET_H
#define PDF_VIEWER_WIDGET_H

#include <QWidget>
#include <QUrl>

class Page_Selector;
class Zoom_Selector;

class QPdfView;
class QPdfDocument;
class QPdfPageNavigator;

QT_BEGIN_NAMESPACE
namespace Ui {
class Pdf_Viewer_Widget;
}
QT_END_NAMESPACE

class Pdf_Viewer_Widget : public QWidget
{
    Q_OBJECT

public:
    Pdf_Viewer_Widget(const QUrl &pdf_location, QWidget *parent = nullptr);
    ~Pdf_Viewer_Widget();

private:
    Ui::Pdf_Viewer_Widget *ui;

    Page_Selector *page_selector;
    Zoom_Selector *zoom_selector;

    QPdfView *pdf_view;                     // pdf를 표시
    QPdfDocument *pdf_document;             // pdf의 데이터를 제공
    QPdfPageNavigator *pdf_page_navigator;  // pdf의 페이지 탐색

private:
    QUrl pdf_location;
    int prev_page_index, current_page_index;
    qreal current_zoom_factor;

private:
    void set_connects();
};
#endif // PDF_VIEWER_WIDGET_H
