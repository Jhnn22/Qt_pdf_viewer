#include "zoom_selector.h"

#include <QVector>
#include <QtMath>

Zoom_Selector::Zoom_Selector(QObject *parent)
    : QObject{parent}
    , zoom_levels{25, 33, 50, 67, 75, 80, 90, 100, 110, 125, 150, 175, 200, 250, 300, 400, 500}
{}

void Zoom_Selector::zoom_select_with_zoom_out_push_button(const qreal current_zoom_factor){
    current_zoom_level_index = find_nearest_zoom_level(current_zoom_factor);

    if(current_zoom_level_index <= 0){
        emit zoom_changed(zoom_levels.first() / 100.0, QString::number(zoom_levels.first()).append('%'));
        return;
    }
    emit zoom_changed(zoom_levels[current_zoom_level_index - 1] / 100.0, QString::number(zoom_levels[current_zoom_level_index - 1]).append('%'));
}

void Zoom_Selector::zoom_select_with_zoom_in_push_button(const qreal current_zoom_factor){
    current_zoom_level_index = find_nearest_zoom_level(current_zoom_factor);

    if(current_zoom_level_index >= zoom_levels.size() - 1){
        emit zoom_changed(zoom_levels.last() / 100.0, QString::number(zoom_levels.last()).append('%'));
        return;
    }
    emit zoom_changed(zoom_levels[current_zoom_level_index + 1] / 100.0, QString::number(zoom_levels[current_zoom_level_index + 1]).append('%'));
}

void Zoom_Selector::zoom_select_with_zoom_line_edit(const QString &input_text){
    qreal zoom_factor = 1.0;
    QString text = "100%";

    QString without_percent(input_text);
    without_percent.remove(QLatin1Char('%'));

    bool ok = false;
    const int zoom_level = without_percent.toInt(&ok);
    if(ok){
        int adjusted_zoom_level = qBound(zoom_levels.first(), zoom_level, zoom_levels.last());
        zoom_factor = adjusted_zoom_level / 100.0;
        text = QString::number(adjusted_zoom_level).append('%');
    }

    emit zoom_changed(zoom_factor, text);
}

int Zoom_Selector::find_nearest_zoom_level(const qreal current_zoom_factor){
    int num = qRound(current_zoom_factor * 100);                             // 정수 값으로 변환
    auto it = std::lower_bound(zoom_levels.begin(), zoom_levels.end(), num); // 가장 가까운 값 확인
    if(it == zoom_levels.end()){
        return zoom_levels.size() - 1;
    }
    int index = it - zoom_levels.begin();

    return index;
}
