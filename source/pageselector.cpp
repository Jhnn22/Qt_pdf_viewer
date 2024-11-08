#include "pageselector.h"
#include <QDebug>

PageSelector::PageSelector(QObject *parent)
    : QObject{parent}
{}

void PageSelector::get_PdfIndexInfo(const int begin, const int end){
    this->begin = begin;
    this->end = end;
}

void PageSelector::pageChanged_with_back_pushButton(const int currentIndex){
    if(currentIndex > begin){
        int prevIndex = currentIndex - 1;
        emit currentIndexChanged(prevIndex);
        emit currentPageChanged(prevIndex + 1);
    }
}

void PageSelector::pageChanged_with_forward_pushButton(const int currentIndex){
    if(currentIndex < end){
        int nextIndex = currentIndex + 1;
        emit currentIndexChanged(nextIndex);
        emit currentPageChanged(nextIndex + 1);
    }
}

void PageSelector::pageChanged(const QString &page_str){
    bool ok = false;
    int page_int = page_str.toInt(&ok);
    if(ok && page_int >= begin + 1 && page_int <= end + 1){
        emit currentIndexChanged(page_int - 1);
        emit currentPageChanged(page_int);
    }
    else{
        emit currentIndexChanged(-1);
    }
}
