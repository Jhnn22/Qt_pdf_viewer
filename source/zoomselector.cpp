#include "zoomselector.h"

#include <QVector>
#include <QtMath>

ZoomSelector::ZoomSelector(QObject *parent)
    : QObject{parent}
    , zoomLevels{25, 33, 50, 67, 75, 80, 90, 100, 110, 125, 150, 175, 200, 250, 300, 400, 500}
{}

void ZoomSelector::zoomSelect_with_zoomOut_pushButton(const qreal currentZoomFactor){
    currentZoomLevelIndex = findNearestZoomLevel(currentZoomFactor);

    if(currentZoomLevelIndex <= 0){
        emit zoomChanged(zoomLevels.first() / 100.0, QString::number(zoomLevels.first()).append('%'));
        return;
    }
    emit zoomChanged(zoomLevels[currentZoomLevelIndex - 1] / 100.0, QString::number(zoomLevels[currentZoomLevelIndex - 1]).append('%'));
}

void ZoomSelector::zoomSelect_with_zoomIn_pushButton(const qreal currentZoomFactor){
    currentZoomLevelIndex = findNearestZoomLevel(currentZoomFactor);

    if(currentZoomLevelIndex >= zoomLevels.size() - 1){
        emit zoomChanged(zoomLevels.last() / 100.0, QString::number(zoomLevels.last()).append('%'));
        return;
    }
    emit zoomChanged(zoomLevels[currentZoomLevelIndex + 1] / 100.0, QString::number(zoomLevels[currentZoomLevelIndex + 1]).append('%'));
}

void ZoomSelector::zoomSelect(const QString &inputText){
    qreal zoomFactor = 1.0;
    QString text = "100%";

    QString withoutPercent(inputText);
    withoutPercent.remove(QLatin1Char('%'));

    bool ok = false;
    const int zoomLevel = withoutPercent.toInt(&ok);
    if(ok){
        int adjustedZoomLevel = qBound(zoomLevels.first(), zoomLevel, zoomLevels.last());
        zoomFactor = adjustedZoomLevel / 100.0;
        text = QString::number(adjustedZoomLevel).append('%');
    }

    emit zoomChanged(zoomFactor, text);
}

int ZoomSelector::findNearestZoomLevel(const qreal currentZoomFactor){
    int num = qRound(currentZoomFactor * 100);                             // 정수 값으로 변환
    auto it = std::lower_bound(zoomLevels.begin(), zoomLevels.end(), num); // 가장 가까운 값 확인
    if(it == zoomLevels.end()){
        return zoomLevels.size() - 1;
    }
    int index = it - zoomLevels.begin();

    return index;
}
