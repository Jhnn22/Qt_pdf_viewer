#ifndef PAGESELECTOR_H
#define PAGESELECTOR_H

#include <QObject>

class PageSelector : public QObject
{
    Q_OBJECT
public:
    explicit PageSelector(QObject *parent = nullptr);

    void get_PdfIndexInfo(const int begin, const int end);

public slots:

    void pageChanged_with_back_pushButton(const int currentIndex);
    void pageChanged_with_forward_pushButton(const int currentIndex);
    void pageChanged(const QString &page_str);

signals:
    void currentIndexChanged(const int changedIndex);
    void currentPageChanged(const int changedPage);

private:
    int begin, end;
};

#endif // PAGESELECTOR_H
