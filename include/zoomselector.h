#ifndef ZOOMSELECTOR_H
#define ZOOMSELECTOR_H

#include <QObject>

class ZoomSelector : public QObject
{
    Q_OBJECT
public:
    explicit ZoomSelector(QObject *parent = nullptr);

public slots:
    void zoomSelectWithZoomOutPushButton(const qreal currentZoomFactor);
    void zoomSelectWithZoomInPushButton(const qreal currentZoomFactor);
    void zoomSelect(const QString &inputText);

signals:
    void zoomChanged(const qreal zoomFactor, const QString &text);

private:
    int currentZoomLevelIndex;

private:
    const QVector<int> zoomLevels;
    int findNearestZoomLevel(const qreal currentZoomFactor);
};

#endif // ZOOMSELECTOR_H
