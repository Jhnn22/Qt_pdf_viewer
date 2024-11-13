#ifndef EVENT_OVERLAY_WIDGET_H
#define EVENT_OVERLAY_WIDGET_H

#include <QWidget>
#include <QObject>

class Event_Overlay_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Event_Overlay_Widget(QWidget *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event);

private:
    QPoint prev_mouse_position, current_mouse_position;
    bool just_pressed, is_dragging;

private:
    void update_pointer_display(const QPoint &prev_mouse_position, const QPoint &current_mouse_position);
};

#endif // EVENT_OVERLAY_WIDGET_H
