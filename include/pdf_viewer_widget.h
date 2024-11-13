#ifndef PDF_VIEWER_WIDGET_H
#define PDF_VIEWER_WIDGET_H

#include <QWidget>
#include <QUrl>

class Page_Selector;
class Zoom_Selector;
class Event_Overlay_Widget;

class QPdfView;
class QPdfDocument;
class QPdfPageNavigator;
class QStackedLayout;

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
    Event_Overlay_Widget *event_overlay_widget; // 이벤트 처리를 위한 투명한 오버레이 위젯

    QPdfView *pdf_view;                         // pdf를 표시
    QPdfDocument *pdf_document;                 // pdf의 데이터를 제공
    QPdfPageNavigator *pdf_page_navigator;      // pdf의 페이지 탐색
    QStackedLayout *stacked_layout;             // 위젯 겹치기를 위한 레이아웃

private:
    QUrl pdf_location;
    int prev_page_index, current_page_index;
    qreal current_zoom_factor;

private:
    void set_connects();
    void set_pdf_viewer();
    void set_stacked_layout();
    void toggle_event_overlay_widget(int paint_mode);
};
#endif // PDF_VIEWER_WIDGET_H
