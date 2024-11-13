#include "event_overlay_widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPoint>

Event_Overlay_Widget::Event_Overlay_Widget(QWidget *parent)
    : QWidget{parent}
    , is_dragging(false)
    , prev_paint_mode(-1), current_paint_mode(-1)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);  // 마우스 이벤트 받기
    setAttribute(Qt::WA_NoSystemBackground, true);          // 배경 투명화
    setWindowFlags(Qt::FramelessWindowHint);                // 프레임 없는 창

    installEventFilter(this);
}

bool Event_Overlay_Widget::eventFilter(QObject *watched, QEvent *event){
    // 마우스 이벤트만 처리
    if(event->type() != QEvent::MouseButtonPress && event->type() != QEvent::MouseMove && event->type() != QEvent::MouseButtonRelease){
        return QWidget::eventFilter(watched, event);
    }

    // 마우스 이벤트로의 타입 변환
    QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);

    // 이벤트 처리
    if(event->type() == QEvent::MouseButtonPress && mouse_event->button() == Qt::LeftButton){
        prev_mouse_position = current_mouse_position = mouse_event->pos();
        is_dragging = true;
    }
    else if(event->type() == QEvent::MouseMove && is_dragging){
        prev_mouse_position = current_mouse_position;
        current_mouse_position = mouse_event->pos();
        qDebug() << current_mouse_position;
        if(current_paint_mode == DRAWING){
            QVector<Line_Info> lines;
            lines.push_back(Line_Info(QLine(prev_mouse_position, current_mouse_position), DRAWING_WIDTH));
            total_lines.push_back(lines);
        }
    }
    else if(event->type() == QEvent::MouseButtonRelease && is_dragging){
        current_mouse_position = mouse_event->pos();
        is_dragging = false;
    }
    update();   // 처리한 이벤트에 대한 화면 갱신

    return QWidget::eventFilter(watched, event);
}

void Event_Overlay_Widget::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QPen pen;
    pen.setBrush(Qt::red);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    if(current_paint_mode == POINTING){

        if(prev_paint_mode == DRAWING){
            for(const auto &lines : total_lines){
                for(const auto &line_info : lines){
                    pen.setWidth(line_info.width);
                    painter.setPen(pen);
                    painter.drawLine(line_info.line);
                }
            }
        }
        if(is_dragging){
            pen.setWidth(POINTING_WIDTH);
            painter.setPen(pen);
            painter.drawPoint(current_mouse_position);
        }
    }
    else if(current_paint_mode == DRAWING){
        for(const auto &lines : total_lines){
            for(const auto &line_info : lines){
                pen.setWidth(line_info.width);
                painter.setPen(pen);
                painter.drawLine(line_info.line);
            }
        }
    }

    QWidget::paintEvent(event);
}

void Event_Overlay_Widget::set_paint_mode(int paint_mode){
    prev_paint_mode = current_paint_mode;
    current_paint_mode = paint_mode;
}

int Event_Overlay_Widget::get_paint_mode(){
    return this->current_paint_mode;
}
