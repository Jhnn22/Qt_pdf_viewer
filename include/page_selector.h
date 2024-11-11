#ifndef PAGE_SELECTOR_H
#define PAGE_SELECTOR_H

#include <QObject>

class Page_Selector : public QObject
{
    Q_OBJECT
public:
    explicit Page_Selector(QObject *parent = nullptr);

    void get_page_index_info(const int begin, const int end);

public slots:

    void page_select_with_back_push_button(const int current_page_index);
    void page_select_with_forward_push_button(const int current_page_index);
    void page_select(const QString &input_text);

signals:
    void page_changed(const int page_index, const QString &text);

private:
    int begin, end;
};

#endif // PAGE_SELECTOR_H
