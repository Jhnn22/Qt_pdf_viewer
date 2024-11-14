#include "page_selector.h"
#include <QDebug>

Page_Selector::Page_Selector(QObject *parent)
    : QObject{parent}
{}

void Page_Selector::set_page_index_info(const int begin, const int end){
    this->begin = begin;
    this->end = end;
}

void Page_Selector::page_select_with_prev_page_push_button(const int current_page_index){
    if(current_page_index > begin){
        int prev_page_index = current_page_index - 1;
        emit page_changed(prev_page_index, QString::number(prev_page_index + 1));
    }
}

void Page_Selector::page_select_with_next_page_push_button(const int current_page_index){
    if(current_page_index < end){
        int next_page_index = current_page_index + 1;
        emit page_changed(next_page_index, QString::number(next_page_index + 1));
    }
}

void Page_Selector::page_select_with_page_line_edit(const QString &input_text){
    bool ok = false;
    int index = input_text.toInt(&ok) - 1;
    if(ok && index >= begin && index <= end){
        emit page_changed(index, input_text);
    }
    else{
        emit page_changed(-1, "");
    }
}
