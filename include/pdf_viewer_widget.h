#ifndef PDF_VIEWER_WIDGET_H
#define PDF_VIEWER_WIDGET_H

#include <QWidget>
#include <QPdfView>
#include <QUrl>

#define SINGLE_PAGE 0
#define MULTI_PAGE  1

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
    Pdf_Viewer_Widget(const QUrl &url, QWidget *parent = nullptr);
    ~Pdf_Viewer_Widget();

    int get_current_page_index();
    int get_total_page_index();
    void page_changed(const int changed_page_index);
    qreal get_current_zoom_factor();
    void zoom_changed(const qreal changed_zoom_factor);
    QPdfView::PageMode get_current_page_mode();
    void page_mode_changed(QPdfView::PageMode changed_page_mode);

signals:
    void update_page_info();
    void update_zoom_info();

private:
    Ui::Pdf_Viewer_Widget *ui;

    QPdfView *pdf_view;                         // pdf를 표시
    QPdfDocument *pdf_document;                 // pdf의 데이터를 제공
    QPdfPageNavigator *pdf_page_navigator;      // pdf의 페이지 탐색
    QUrl url;
    void set_connects();
    void set_pdf_viewer();

    bool using_tool_bar;
    int prev_page_index, current_page_index;
};
#endif // PDF_VIEWER_WIDGET_H
