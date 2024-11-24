#include "main_window.h"
#include "websocket_server.h"

#include <QApplication>
#include <QThread>

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
        qDebug() << message;
        manual(&w, message);

    });

    serverThread->start();

    return app.exec();
}

void manual(Main_Window *w, const QString &message){
    if(message == "left")       w->action_prev_page_triggered();
    else if(message == "right") w->action_next_page_triggered();
}
