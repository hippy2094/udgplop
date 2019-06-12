#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "appvars.h"
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QStringList>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimer>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    uchar pixels[7];
    QGridLayout *blayout;
    QPushButton *buttons[64];
    QToolButton *btnNew;
    QToolButton *btnOpen;
    QToolButton *btnSave;
    QToolButton *btnAbout;
    void UpdateTextView();
    void SetButton(int x, int y, uchar v);
    bool IsSaved;
    QString CurrentFile;
    QLabel *labelUpdate;

public slots:
    void closeEvent(QCloseEvent *event);
    void PixelButtonClick();
    void OpenFileClick();
    void SaveFileClick();
    void NewClick();
    void AboutClick();
    void CheckUpdates();
};

#endif // MAINWINDOW_H
