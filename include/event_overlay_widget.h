#ifndef EVENT_OVERLAY_WIDGET_H
#define EVENT_OVERLAY_WIDGET_H

#include <QWidget>
#include <QObject>

#define POINTING 0
#define POINTING_WIDTH 10
#define DRAWING 1
#define DRAWING_WIDTH 5

class Event_Overlay_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Event_Overlay_Widget(QWidget *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void remove_total_lines();

public slots:
    void set_paint_mode(int mode);
    int get_paint_mode();

private:
    QPoint prev_mouse_position, current_mouse_position;
    bool is_dragging;
    int prev_paint_mode, current_paint_mode;

private:
    void draw_prev_lines(QPainter &painter, QPen &pen);
    void draw_current_lines(QPainter &painter, QPen &pen);

private:
    struct Line_Info{
        QLine line;
        int width;
        Line_Info(const QLine &line, const int width){
            this->line = line;
            this->width = width;
        }
    };
    QVector<QVector<Line_Info>> total_lines;
    QVector<Line_Info> lines;
};

#endif // EVENT_OVERLAY_WIDGET_H
