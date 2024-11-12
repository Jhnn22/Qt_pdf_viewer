#ifndef ZOOM_SELECTOR_H
#define ZOOM_SELECTOR_H

#include <QObject>

class Zoom_Selector : public QObject
{
    Q_OBJECT
public:
    explicit Zoom_Selector(QObject *parent = nullptr);

public slots:
    void zoom_select_with_zoom_out_push_button(const qreal current_zoom_factor);
    void zoom_select_with_zoom_in_push_button(const qreal current_zoom_factor);
    void zoom_select_with_zoom_line_edit(const QString &input_text);

signals:
    void zoom_changed(const qreal changed_zoom_factor, const QString &changed_text);

private:
    int current_zoom_level_index;

private:
    const QVector<int> zoom_levels;
    int find_nearest_zoom_level(const qreal current_zoom_factor);
};

#endif // ZOOM_SELECTOR_H
