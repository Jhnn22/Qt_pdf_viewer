#include "main_window.h"
#include "websocket_server.h"

#include <QApplication>
#include <QThread>

#include <QRegularExpression>

void manual(Main_Window *w, const QString &message);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Main_Window w;
    w.show();

    net::io_context ioc;

    WebSocketServer *server = new WebSocketServer(ioc, tcp::endpoint{ net::ip::make_address("0.0.0.0"), 8080 });

    QThread *serverThread = new QThread;
    server->moveToThread(serverThread);

    // ioc 캡쳐 문제...?
    QObject::connect(serverThread, &QThread::started, [&ioc]() {
        ioc.run();
    });

    QObject::connect(server, &WebSocketServer::message_received, &w, [&w](const QString &message){
        manual(&w, message);
    });

    serverThread->start();

    return app.exec();
}

void manual(Main_Window *w, const QString &message){
    static const QRegularExpression re("^(\\d+),(\\d+)$");
    QRegularExpressionMatch match = re.match(message);
    if(match.hasMatch()){
        int x = match.captured(1).toInt();
        int y = match.captured(2).toInt();
        w->set_pos(x, y);

        return;
    }

    if(message == "left"){
        w->action_prev_page_triggered();
    }
    else if(message == "right"){
        w->action_next_page_triggered();
    }
    else if(message == "point" || message == "draw"){
        w->set_paint_mode(
            message == "point" ? 0 : 1 // 0: POINTING, 1: DRAWING
        );
    }
}
