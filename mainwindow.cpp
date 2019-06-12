#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setMaximumSize(763,547);
    this->setWindowTitle("UDG Plop [Untitled]");
    IsSaved = false;
    CurrentFile = "Untitled";
    int i,j,c;
    for(i=0;i<8;i++) {
        pixels[i] = 0;
    }
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->outputView->setFont(fixedFont);
    ui->mainViewWidget->setMaximumWidth(420);
    ui->mainViewWidget->setMaximumHeight(420);
    blayout = new QGridLayout(this);
    blayout->setSpacing(5);
    c = 0;
    for(i=0;i<8;i++) {
        for(j=0;j<8;j++) {
            buttons[c] = new QPushButton(this);
            buttons[c]->setText(QString::number(i)+","+QString::number(j));
            buttons[c]->setMinimumSize(24,24);
            buttons[c]->setFixedSize(50,50);
            buttons[c]->setCheckable(true);
            connect(buttons[c],SIGNAL(clicked()),this,SLOT(PixelButtonClick()));
            buttons[c]->setStyleSheet("\
                            QPushButton { color: white; background-color: white; border: none}\
                            QPushButton:checked{\
                                color: black; background-color: black;\
                                border: none; \
                            }\
                            QPushButton:hover{  \
                                color: #808080; background-color: #808080; \
                                border-style: outset;  \
                            }  \
                            ");
            blayout->addWidget(buttons[c],i,j);
            c++;
        }
    }
    ui->mainViewWidget->setLayout(blayout);
    btnNew = new QToolButton(this);
    btnNew->setText("New");
    connect(btnNew,SIGNAL(clicked()),this,SLOT(NewClick()));
    ui->mainToolBar->addWidget(btnNew);
    btnOpen = new QToolButton(this);
    btnOpen->setText("Open ");
    connect(btnOpen,SIGNAL(clicked()),this,SLOT(OpenFileClick()));
    ui->mainToolBar->addWidget(btnOpen);
    btnSave = new QToolButton(this);
    btnSave->setText("Save");
    connect(btnSave,SIGNAL(clicked()),this,SLOT(SaveFileClick()));
    ui->mainToolBar->addWidget(btnSave);
    UpdateTextView();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::PixelButtonClick() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QStringList parts;
    parts << button->text().split(",");
    int i = parts[0].toInt();
    int j = parts[1].toInt();
    pixels[i] ^= 1UL << j;
    UpdateTextView();
    IsSaved = false;
}

void MainWindow::UpdateTextView() {
    ui->outputView->clear();
    QString output;
    int i,j,c;
    for(c=0;c<8;c++) {
        for(i=0;i<8;i++) {
            j = (pixels[c] >> i) & 1;
            output += QString::number(j);
        }
        output += " " + QString::number(pixels[c]) + "\n";
    }
    ui->outputView->setPlainText(output);
}

void MainWindow::OpenFileClick() {
    if(!IsSaved) {
        if(QMessageBox(QMessageBox::Information, "Unsaved file", "Do you wish to save this file?", QMessageBox::Yes|QMessageBox::No).exec() == QMessageBox::Yes) {
            SaveFileClick();
        }
    }
    QString filename = QFileDialog::getOpenFileName(this, "Open file", "", "*.udgp");
    if(filename.isEmpty()) return;
    QFile f(filename);
    QString contents;
    if(f.open(QIODevice::ReadOnly)) {
        QTextStream instream(&f);
        contents.append(instream.readAll());
        f.close();
    }
    if(!contents.isEmpty()) {
        QStringList lines;
        QStringList parts;
        lines << contents.split("\n");
        int i,j,c;
        for(i=0;i<lines.count();i++) {
            parts << lines.at(i).split(" ");
            if(parts.count() == 2) {
                QRegExp re("\\d*");
                if(re.exactMatch(parts.at(1))) {
                    pixels[i] = static_cast<uchar>(parts.at(1).toUInt());
                }
                else {
                    pixels[i] = 0;
                }
            }
            parts.clear();
        }
        UpdateTextView();
        for(c=0;c<8;c++) {
            for(i=0;i<8;i++) {
                j = (pixels[c] >> i) & 1;
                SetButton(c,i,static_cast<uchar>(j));
            }
        }
        IsSaved = true;
        this->setWindowTitle("UDG Plop ["+filename+"]");
        CurrentFile = filename;
    }
}

void MainWindow::SaveFileClick() {
    QString outputfile;
    if(CurrentFile.isEmpty() || CurrentFile == "Untitled") {
      outputfile = QFileDialog::getSaveFileName(this, "Choose file","","*.udgp");
    }
    else outputfile = CurrentFile;
    if(outputfile.isEmpty()) return;
    if(!outputfile.endsWith(".udgp")) outputfile.append(".udgp");
    QFile f(outputfile);
    if(f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out << ui->outputView->toPlainText();
        f.close();
        IsSaved = true;
        this->setWindowTitle("UDG Plop ["+outputfile+"]");
        CurrentFile = outputfile;
    }
}

void MainWindow::NewClick() {
    if(!IsSaved) {
        if(QMessageBox(QMessageBox::Information, "Unsaved file", "Do you wish to save this file?", QMessageBox::Yes|QMessageBox::No).exec() == QMessageBox::Yes) {
            SaveFileClick();
        }
    }
    int i;
    for(i=0;i<8;i++) {
        pixels[i] = 0;
    }
    for(i=0;i<64;i++) {
        buttons[i]->setChecked(false);
    }
    IsSaved = false;
    UpdateTextView();
}

void MainWindow::SetButton(int x, int y, uchar v) {
    int i;
    QString cheatstring = QString::number(x) + "," + QString::number(y);
    for(i=0;i<64;i++) {
        QPushButton* button = qobject_cast<QPushButton*>(buttons[i]);
        if(button->text() == cheatstring) {
            if(v == 0) button->setChecked(false);
            else button->setChecked(true);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(!IsSaved) {
        if(QMessageBox(QMessageBox::Information, "Unsaved file", "Do you wish to save this file?", QMessageBox::Yes|QMessageBox::No).exec() == QMessageBox::Yes) {
            SaveFileClick();
        }
    }
}
