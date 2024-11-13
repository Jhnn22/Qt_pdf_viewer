#ifndef PAGE_SELECTOR_H
#define PAGE_SELECTOR_H

#include <QObject>

class Page_Selector : public QObject
{
    Q_OBJECT
public:
    explicit Page_Selector(QObject *parent = nullptr);

    void set_page_index_info(const int begin, const int end);

public slots:

    void page_select_with_prev_push_button(const int current_page_index);
    void page_select_with_next_push_button(const int current_page_index);
    void page_select_with_page_line_edit(const QString &input_text);

signals:
    void page_changed(const int changed_page_index, const QString &changed_text);

private:
    int begin, end;
};

#endif // PAGE_SELECTOR_H
