#include "event_overlay_widget.h"

#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QTimer>


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
            paths.push_back(QVector<QLine>());  // 새로운 벡터 추가

            // 투명도 초기화 및 타이머 일시 정지
            if(timer->isActive()){
                color_opacity = 1.0;
                update();
                timer->stop();
            }
        }
    }
    else if(event->type() == QEvent::MouseMove && is_dragging){
        prev_mouse_position = current_mouse_position;
        current_mouse_position = mouse_event->pos();

        if(current_paint_mode == DRAWING){
            paths.last().push_back(QLine(prev_mouse_position, current_mouse_position));
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
            if(!path.empty()){
                QPainterPath painter_path;
                painter_path.moveTo(path.first().p1());
                for(const auto &line : path){
                    painter_path.lineTo(line.p2());
                }
                painter.drawPath(painter_path);
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
        if(color_opacity > 0.01){
            color_opacity -= 0.01;
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
            timer->start(10);
        }
    });
}
