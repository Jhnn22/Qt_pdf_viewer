#include "main_window.h"
#include "ui_main_window.h"

#include "pdf_viewer_widget.h"
#include "event_overlay_widget.h"

#include <QLineEdit>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPointF>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMetaObject>
#include <QMetaMethod>

Main_Window::Main_Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Main_Window)
    , pdf_dialog(nullptr)
    , event_overlay_widget(new Event_Overlay_Widget(nullptr))
{
    ui->setupUi(this);
    set_pdf_list();
    set_tool_bar();
    set_connects();

}

Main_Window::~Main_Window()
{
    delete ui;
}

bool Main_Window::eventFilter(QObject *obj, QEvent *event){
    if(event->type() == QEvent::Enter){
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if(widget){
            hash_2.value(widget)->setVisible(true);
        }
    }
    else if(event->type() == QEvent::Leave){
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if(widget){
            hash_2.value(widget)->setVisible(false);
        }
    }
    emit f();

    return QWidget::eventFilter(obj, event);
}

void Main_Window::set_pdf_list(){
    pdf_list_layout = qobject_cast<QVBoxLayout*>(ui->widget->layout());
    if(pdf_list_layout){
        pdf_list_layout->setContentsMargins(0, 0, 0, 0);
        pdf_list_layout->setAlignment(Qt::AlignTop);


    }
}

void Main_Window::set_tool_bar(){
    // 아래 정렬을 위한 스페이서
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 페이지
    QWidget *widget = new QWidget(this);
    QVBoxLayout *vertical_layout = new QVBoxLayout(widget);
    vertical_layout->setContentsMargins(0, 0, 0, 0);

    page_line_edit = new QLineEdit(this);
    page_line_edit->setFixedSize(30, 30);
    page_line_edit->setFrame(false);
    page_line_edit->setAlignment(Qt::AlignCenter);
    page_line_edit->setToolTip("특정 페이지 숫자로 이동");

    total_page_label = new QLabel(this);
    total_page_label->setFixedSize(30, 30);
    total_page_label->setAlignment(Qt::AlignCenter);
    total_page_label->setToolTip("이 PDF의 총 페이지 수");

    vertical_layout->addWidget(page_line_edit, 0, Qt::AlignCenter);
    vertical_layout->addWidget(total_page_label, 0, Qt::AlignCenter);

    action_prev_page = new QAction(this);
    action_prev_page->setText("▲");
    action_prev_page->setToolTip("이전 페이지로 이동");

    action_next_page = new QAction(this);
    action_next_page->setText("▼");
    action_next_page->setToolTip("다음 페이지로 이동");

    action_full_screen = new QAction(this);
    action_full_screen->setText("발표");
    action_full_screen->setToolTip("전체 화면");

    // 순서에 맞게 툴 바에 추가
    ui->tool_bar->addWidget(spacer);
    ui->tool_bar->addWidget(widget);
    ui->tool_bar->addAction(action_prev_page);
    ui->tool_bar->addAction(action_next_page);
    ui->tool_bar->addAction(action_full_screen);
}

void Main_Window::set_connects(){
    // pdf 목록 불러오기
    connect(ui->load_push_button, &QPushButton::clicked, this, &Main_Window::load_push_button_clicked);

    // 툴바
    // pdf 추가 및 변경 시 정보 업데이트
    connect(this, &Main_Window::current_widget_changed, this, [this](const QString &name){
        focused_widget = hash.value(name);
        focused_pdf_viewer_widget = focused_widget->findChild<Pdf_Viewer_Widget*>();

        if(!focused_widget || !focused_pdf_viewer_widget){
            return;
        }

        current_page_index = focused_pdf_viewer_widget->get_current_page_index();
        page_line_edit->setText(QString::number(current_page_index + 1));
        total_page_index = focused_pdf_viewer_widget->get_total_page_index();
        total_page_label->setText(QString::number(total_page_index + 1));
        set_name(name);

        qDebug() << "---------------------------------------------------"
                 << "\nloaded or changed widget info"
                 << "\nname  :" << name
                 << "\nwidget:" << focused_widget;
    });
    // 페이지
    connect(page_line_edit, &QLineEdit::returnPressed, this, [this](){
        QString input_text = page_line_edit->text();
        page_line_edit_return_pressed(input_text);
    });
    connect(action_prev_page, &QAction::triggered, this, &Main_Window::action_prev_page_triggered);
    connect(action_next_page, &QAction::triggered, this, &Main_Window::action_next_page_triggered);
    connect(action_full_screen, &QAction::triggered, this, &Main_Window::action_full_screen_triggered);
    connect(event_overlay_widget, &Event_Overlay_Widget::restore_from_full_screen, this, &Main_Window::restore_from_full_screen);
}

void Main_Window::load_push_button_clicked(){
    if(pdf_dialog == nullptr){
        pdf_dialog = new QFileDialog(this, tr("open pdf"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));   // 기본 경로 : 다운로드
        pdf_dialog->setAcceptMode(QFileDialog::AcceptOpen);   // 열기 모드
        pdf_dialog->setMimeTypeFilters({"application/pdf"});  // pdf 문서만 표시
    }
    if(pdf_dialog->exec() == QDialog::Accepted){
        const QUrl url = pdf_dialog->selectedUrls().constFirst();
        if(url.isValid()){
            open_pdf(url);
        }
    }
}

void Main_Window::open_pdf(const QUrl &url){
    if(url.isLocalFile()){
        Pdf_Viewer_Widget *pdf_viewer_widget = new Pdf_Viewer_Widget(url, this);

        int num = 2;
        QString original_name = url.fileName().remove(".pdf");
        QString name = original_name;
        while(hash.contains(name)){
            name = QString("%1_%2").arg(original_name).arg(num++);
        }

        make_widget(pdf_viewer_widget, name);
        make_button(name);

        emit current_widget_changed(name);

        connect(pdf_viewer_widget, &Pdf_Viewer_Widget::update_page_line_edit, this, [this, pdf_viewer_widget](){
            current_page_index = pdf_viewer_widget->get_current_page_index();
            qDebug() << "current page:" << current_page_index + 1;
            page_line_edit->setText(QString::number(current_page_index + 1));
        });
    }
    else{
        qDebug() << "failed to open";
        return;
    }
}

void Main_Window::make_widget(Pdf_Viewer_Widget *pdf_viewer_widget, const QString &name){
    QWidget *widget = new QWidget(ui->stacked_widget);

    QStackedLayout *stacked_layout = new QStackedLayout(widget);
    stacked_layout->setStackingMode(QStackedLayout::StackAll);
    stacked_layout->setContentsMargins(0, 0, 0, 0);
    stacked_layout->addWidget(pdf_viewer_widget);

    widget->setLayout(stacked_layout);

    ui->stacked_widget->addWidget(widget);
    ui->stacked_widget->setCurrentWidget(widget);

    hash.insert(name, widget);
}

void Main_Window::make_button(const QString &name){
    QWidget *widget = new QWidget(ui->widget);
    widget->installEventFilter(this);

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPushButton *button = new QPushButton(widget);
    button->setMaximumWidth(150);
    button->setFixedHeight(30);
    button->setObjectName(name);

    QPushButton *button_2= new QPushButton(widget);
    button_2->setText("✕");
    button_2->setFixedSize(30, 30);
    button_2->setStyleSheet(
        "text-align: center;"
    );
    button_2->setVisible(false);

    layout->addWidget(button);
    layout->addWidget(button_2);

    widget->setLayout(layout);

    pdf_list_layout->addWidget(widget);

    QMetaObject::invokeMethod(this, [this, name, button]{
        int width = button->width();
        QString elided_name = button->fontMetrics().elidedText(name, Qt::ElideRight, width - 10);  // 여유 값 10
        button->setText(elided_name);
    }, Qt::QueuedConnection);

    connect(this, &Main_Window::f, this, [this, name, button](){
        int width = button->width();
        QString elided_name = button->fontMetrics().elidedText(name, Qt::ElideRight, width - 10);
        button->setText(elided_name);
    });
    connect(button, &QPushButton::clicked, this, [this, name]{
        QWidget *named_widget = hash.value(name);
        QWidget *current_widget = ui->stacked_widget->currentWidget();

        if(!named_widget || !current_widget){
            return;
        }

        if(current_widget != named_widget){
            ui->stacked_widget->setCurrentWidget(named_widget);

            emit current_widget_changed(name);
        }
    });
    connect(button_2, &QPushButton::clicked, this, [this, button](){
        qDebug() << button;
    });

    hash_2.insert(widget, button_2);
}

void Main_Window::set_name(const QString &name){
    this->name = name;
}

QString Main_Window::get_name(){
    return name;
}

void Main_Window::page_line_edit_return_pressed(const QString &input_text){
    bool ok = false;
    int index = input_text.toInt(&ok) - 1;
    if(ok && index >= 0 && index <= total_page_index){
        focused_pdf_viewer_widget->page_changed(index);
    }
    page_line_edit->setText(QString::number(focused_pdf_viewer_widget->get_current_page_index() + 1));
}

void Main_Window::action_prev_page_triggered(){
    if(current_page_index > 0){
        int prev_page_index = current_page_index - 1;
        focused_pdf_viewer_widget->page_changed(prev_page_index);
    }
}

void Main_Window::action_next_page_triggered(){
    if(current_page_index < total_page_index){
        int next_page_index = current_page_index + 1;
        focused_pdf_viewer_widget->page_changed(next_page_index);
    }
}

void Main_Window::action_full_screen_triggered(){
    focused_pdf_viewer_widget->set_page_mode(QPdfView::PageMode::SinglePage);

    focused_widget->layout()->addWidget(event_overlay_widget);
    event_overlay_widget->show();
    event_overlay_widget->raise();

    ui->stacked_widget->removeWidget(focused_widget);
    ui->stacked_widget->setCurrentIndex(0);
    this->hide();
    focused_widget->setParent(nullptr);
    focused_widget->showFullScreen();
    event_overlay_widget->setFocus();
}

void Main_Window::restore_from_full_screen(){
    focused_pdf_viewer_widget->set_page_mode(QPdfView::PageMode::MultiPage);

    focused_widget->layout()->removeWidget(event_overlay_widget);
    event_overlay_widget->setParent(nullptr);
    event_overlay_widget->set_paint_mode(-1);
    event_overlay_widget->hide();

    this->show();
    ui->stacked_widget->addWidget(focused_widget);
    ui->stacked_widget->setCurrentWidget(focused_widget);

    emit current_widget_changed(name);
}

void Main_Window::set_paint_mode(int paint_mode){
    event_overlay_widget->set_paint_mode(paint_mode);
}

void Main_Window::set_pos(const int x, const int y){
    event_overlay_widget->set_pos(x, y);
}
