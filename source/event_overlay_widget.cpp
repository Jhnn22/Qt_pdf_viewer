#include "event_overlay_widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPainterPath>
#include <QTimer>
#include <QPointF>
#include <algorithm>


Event_Overlay_Widget::Event_Overlay_Widget(QWidget *parent)
    : QWidget{parent}
    , is_dragging(false)
    , prev_paint_mode(-1), current_paint_mode(-1)
    , color_opacity(1.0)
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
        is_dragging = true;
        prev_mouse_position = current_mouse_position = mouse_event->pos();

        if(current_paint_mode == DRAWING){
            // 투명도 초기화 및 타이머 일시 정지
            if(timer->isActive()){
                color_opacity = 1.0;
                update();
                timer->stop();
            }

            // 새로운 드로잉 경로 추가
            path = new QPainterPath();
            path->moveTo(current_mouse_position);
            paths.push_back(path);
        }
    }
    else if(event->type() == QEvent::MouseMove && is_dragging){
        prev_mouse_position = current_mouse_position;
        current_mouse_position = mouse_event->pos();

        if(current_paint_mode == DRAWING && path){
            // 베지어 곡선 방식?
            QPointF point = (prev_mouse_position + current_mouse_position) / 2.0;
            path->quadTo(prev_mouse_position, point);
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
    // painter 설정
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 안티앨리어싱 활성화
    // pen 설정
    QPen pen;
    pen.setBrush(Qt::red);
    pen.setWidth(current_paint_mode == POINTING ? POINTING_WIDTH : DRAWING_WIDTH);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::BevelJoin);

    if(current_paint_mode == POINTING && is_dragging){
        painter.setPen(pen);
        painter.drawPoint(current_mouse_position);
    }
    else if(current_paint_mode == DRAWING){
        // 투명도 설정
        QColor color = Qt::red;
        color.setAlphaF(color_opacity);
        pen.setBrush(color);
        painter.setPen(pen);

        for(const auto &path : paths){
            if(path){
                painter.drawPath(*path);
            }
        }
    }

    QWidget::paintEvent(event);
}

void Event_Overlay_Widget::set_paint_mode(int paint_mode){
    prev_paint_mode = current_paint_mode;
    current_paint_mode = paint_mode;
}

void Event_Overlay_Widget::set_connects(){
    // 라인 삭제 설정--------------------------------------------------------------
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        if(color_opacity > 0.03){
            color_opacity -= 0.03;
        }
        else{
            timer->stop();
            paths.clear();
            color_opacity = 1.0;
        }
        update();
    });
    connect(this, &Event_Overlay_Widget::drawing_finished, this, [this]{
        if(!is_dragging && !paths.empty()){
            timer->start(16);   // ~60fps
        }
    });
}
