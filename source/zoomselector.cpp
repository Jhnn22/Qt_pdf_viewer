#include "zoomselector.h"

ZoomSelector::ZoomSelector(QObject *parent)
    : QObject{parent}
{}

void ZoomSelector::ratioChanged_with_zoomOut_pushButton(){

}

void ZoomSelector::ratioChanged_with_zoomIn_pushButton(){

}

void ZoomSelector::ratioChanged(const QString &ratio){
    qreal factor = 1.0;

    QString ratio_copy(ratio);
    ratio_copy.remove(QLatin1Char('%'));   // ratio의 복사본에서 % 제거

    bool ok = false;
    const int zoomLevel = ratio_copy.toInt(&ok);
    if(ok){
        factor = zoomLevel / 100.0;
    }

    emit zoomFactorChanged(factor);
}
