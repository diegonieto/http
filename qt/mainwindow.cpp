#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    connect(&socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(&socket, SIGNAL(readyRead()), this, SLOT(onSocketRead()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_download_clicked()
{
    const QUrl url(ui->address->toPlainText());
    log("Url: " + url.toString());
    const QString hostToLookUp = url.host();
    log("DNS Look up for: " + hostToLookUp);
        if (hostToLookUp.size()) {
        QHostInfo::lookupHost(hostToLookUp,
                              this, SLOT(lookedUp(QHostInfo)));
    }
}

void MainWindow::lookedUp(const QHostInfo &host)
{
    if (host.error() != QHostInfo::NoError) {
        log("Lookup failed: " + host.errorString());
        return;
    }

    for (const QHostAddress &address : host.addresses())
    {
        log("Found address: " + address.toString());
        log("Connecting to socket");
        if (socket.state() == QAbstractSocket::ConnectedState) {
            socket.disconnectFromHost();
        }
        socket.connectToHost(address, 80);
    }
}

void MainWindow::onSocketDisconnected()
{
    log("Socket was disconnected");
}

void MainWindow::onSocketRead()
{
    QByteArray socketData = socket.readAll();
    log("Reading from socket " + QString::number(socketData.size()) + " bytes");
    ui->websiteView->textCursor().insertText(QString(socketData));
    ui->websiteView->moveCursor(QTextCursor::End);
}

void MainWindow::onSocketConnected()
{
    log("socket connected");
    QString const request = generateGet();
    log("\n\nFull request:\n" + request);

    ui->websiteView->textCursor().insertText("========== NEW REPLY ========== ");
    log("Writing to socket the request");
    socket.write(request.toLatin1().data());

    log("The request was written");
}

void MainWindow::log(const QString log) const
{
    ui->logOutput->textCursor().insertText(log + "\n");
    ui->logOutput->moveCursor(QTextCursor::End);
}

const QString MainWindow::generateGet() const
{
    const QUrl url(ui->address->toPlainText());
    const QString path = url.path();
    log("Setting path: " + path);
    const QString HTTPHeader = " HTTP/1.1\r\n";
    log("Setting HTTP header: " + HTTPHeader);
    const QString connectionHeader = "Connection: Keep-Alive\r\n";
    log("Setting connection header: " + connectionHeader);
    const QString encodingHeader = "Accept-Encoding: gzip, deflate\r\n";
    log("Setting encoding header: " + encodingHeader);
    const QString languageHeader = "Accept-Language: es-ES,en,*\r\n";
    log("Setting language header: " + languageHeader);
    const QString userAgentHeader = "User-Agent: Mozilla/5.0\r\n";
    log("Setting user agent header: " + userAgentHeader);
    const QString hostHeader = "Host: " + url.host() + "\r\n";
    log("Setting host header: " + userAgentHeader);
    QString fullRequest = "GET " + path + HTTPHeader + connectionHeader
                          + encodingHeader + languageHeader + userAgentHeader
                          + hostHeader + "\r\n";
    return fullRequest;
}

void MainWindow::on_clean_clicked()
{
    ui->address->clear();
    ui->logOutput->clear();
    ui->websiteView->clear();
}
