#include "pageselector.h"
#include <QDebug>

PageSelector::PageSelector(QObject *parent)
    : QObject{parent}
{}

void PageSelector::getPdfIndexInfo(const int begin, const int end){
    this->begin = begin;
    this->end = end;
}

void PageSelector::pageSelect_with_back_pushButton(const int currentPageIndex){
    if(currentPageIndex > begin){
        int prevPageIndex = currentPageIndex - 1;
        emit pageChanged(prevPageIndex, QString::number(prevPageIndex + 1));
    }
}

void PageSelector::pageSelect_with_forward_pushButton(const int currentPageIndex){
    if(currentPageIndex < end){
        int nextPageIndex = currentPageIndex + 1;
        emit pageChanged(nextPageIndex, QString::number(nextPageIndex + 1));
    }
}

void PageSelector::pageSelect(const QString &inputText){
    bool ok = false;
    int text = inputText.toInt(&ok);
    if(ok && text >= begin + 1 && text <= end + 1){
        emit pageChanged(text - 1, inputText);
    }
    else{
        emit pageChanged(-1, "");
    }
}
