#ifndef PAGESELECTOR_H
#define PAGESELECTOR_H

#include <QObject>

class PageSelector : public QObject
{
    Q_OBJECT
public:
    explicit PageSelector(QObject *parent = nullptr);

    void getPdfIndexInfo(const int begin, const int end);

public slots:

    void pageSelect_with_back_pushButton(const int currentPageIndex);
    void pageSelect_with_forward_pushButton(const int currentPageIndex);
    void pageSelect(const QString &inputText);

signals:
    void pageChanged(const int pageIndex, const QString &text);

private:
    int begin, end;
};

#endif // PAGESELECTOR_H
