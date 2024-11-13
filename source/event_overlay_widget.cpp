#include "event_overlay_widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>

Event_Overlay_Widget::Event_Overlay_Widget(QWidget *parent)
    : QWidget{parent}
    , just_pressed(false), is_dragging(false)
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
    switch(event->type()){
    case QEvent::MouseButtonPress:
        if(mouse_event->button() == Qt::LeftButton){
            update_pointer_display(mouse_event->pos(), mouse_event->pos());
            just_pressed = true;

        }
        break;
    case QEvent::MouseMove:
        if(is_dragging){
            update_pointer_display(current_mouse_position, mouse_event->pos());

        }
        break;
    case QEvent::MouseButtonRelease:
        if(is_dragging){
            update_pointer_display(current_mouse_position, mouse_event->pos());
            is_dragging = false;
        }
        break;
    }

    return QWidget::eventFilter(watched, event);
}

void Event_Overlay_Widget::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QPen pen(Qt::red, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);

    // 시작 위치 표시
    if(just_pressed){
        painter.drawPoint(current_mouse_position);
        just_pressed = false;
        is_dragging = true;
    }
    // 현재 위치 표시, 자연스러운 연결을 위해 라인으로 표현
    else if(!just_pressed && is_dragging){
        painter.drawLine(prev_mouse_position, current_mouse_position);
    }

    QWidget::paintEvent(event);
}

void Event_Overlay_Widget::update_mouse_position(const QPoint &prev_mouse_position, const QPoint &current_mouse_position){
    this->prev_mouse_position = prev_mouse_position;
    this->current_mouse_position = current_mouse_position;
    update();
}
