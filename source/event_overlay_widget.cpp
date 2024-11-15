#include "event_overlay_widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QTimer>

Event_Overlay_Widget::Event_Overlay_Widget(QWidget *parent)
    : QWidget{parent}
    , is_dragging(false)
    , prev_paint_mode(-1), current_paint_mode(-1)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);  // 마우스 이벤트 받기
    setAttribute(Qt::WA_NoSystemBackground, true);          // 배경 투명화
    setWindowFlags(Qt::FramelessWindowHint);                // 프레임 없는 창
    setFocusPolicy(Qt::StrongFocus);

    set_connects();

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
        if(current_paint_mode == DRAWING){
            lines.enqueue(QLine(prev_mouse_position, current_mouse_position));
        } 
    }
    else if(event->type() == QEvent::MouseButtonRelease && is_dragging){
        is_dragging = false;
        if(current_paint_mode == DRAWING){
            emit drawing_finished();
        }
    }
    update();

    return QWidget::eventFilter(watched, event);
}

void Event_Overlay_Widget::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QPen pen;
    pen.setBrush(Qt::red);
    pen.setWidth(current_paint_mode == POINTING ? POINTING_WIDTH : DRAWING_WIDTH);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    if(current_paint_mode == POINTING && is_dragging){
        painter.setPen(pen);
        painter.drawPoint(current_mouse_position);
    }
    else if(current_paint_mode == DRAWING){
        painter.setPen(pen);
        for(const auto &line : lines){
            painter.drawLine(line);
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

void Event_Overlay_Widget::set_connects(){
    // 라인 지우개 설정--------------------------------------------------------------
    timer = new QTimer(this);
    timer->setInterval(16);  // 약 60fps
    connect(timer, &QTimer::timeout, this, [this](){
        if(!lines.empty()){
            lines.pop_front();
            update();
        }
        else{
            timer->stop();
        }
    });
    connect(this, &Event_Overlay_Widget::drawing_finished, this, [this]{
        // 버튼 릴리즈 이벤트 이후 타이머 시작
        if(!is_dragging && !lines.empty()){
            timer->start();
        }
    });
}
