#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QStringList>
#include <QToolButton>
#include <QFileDialog>
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
    void UpdateTextView();
    void SetButton(int x, int y, uchar v);
    bool IsSaved;

public slots:
    void PixelButtonClick();
    void OpenFileClick();
    void SaveFileClick();
    void NewClick();
};

#endif // MAINWINDOW_H
