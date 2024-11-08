#ifndef ZOOMSELECTOR_H
#define ZOOMSELECTOR_H

#include <QObject>

class ZoomSelector : public QObject
{
    Q_OBJECT
public:
    explicit ZoomSelector(QObject *parent = nullptr);

public slots:
    void ratioChanged_with_zoomOut_pushButton();
    void ratioChanged_with_zoomIn_pushButton();
    void ratioChanged(const QString &ratio);


signals:
    void zoomFactorChanged(qreal zoomFactor);
};

#endif // ZOOMSELECTOR_H
