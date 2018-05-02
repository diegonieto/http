#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDnsLookup>
#include <QHostInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void log(const QString log) const;
    const QString generateGet() const;

private slots:
    void on_download_clicked();
    void lookedUp(const QHostInfo &host);
    void onSocketConnected();
    void onSocketRead();
    void onSocketDisconnected();

    void on_clean_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket socket;
};

#endif // MAINWINDOW_H
