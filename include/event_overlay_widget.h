#ifndef EVENT_OVERLAY_WIDGET_H
#define EVENT_OVERLAY_WIDGET_H

#include <QWidget>
#include <QObject>

#define POINTING 0
#define POINTING_WIDTH 10
#define DRAWING 1
#define DRAWING_WIDTH 5

class QTimer;

class Event_Overlay_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Event_Overlay_Widget(QWidget *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

public slots:
    void set_paint_mode(int mode);

signals:
    void drawing_finished();

private:
    QPoint prev_mouse_position, current_mouse_position;
    bool is_dragging;
    int prev_paint_mode, current_paint_mode;
    QVector<QVector<QLine>> paths;
    QTimer *timer;
    qreal color_opacity;

    void set_connects();
};

#endif // EVENT_OVERLAY_WIDGET_H
