#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef Q_OS_WIN
QString GetWinVer() {
    // http://web3.codeproject.com/Messages/4823136/Re-Another-Way.aspx
    NTSTATUS (WINAPI *RtlGetVersion)(LPOSVERSIONINFOEXW);
    OSVERSIONINFOEXW osInfo;

    *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");
    QString result;
    if (NULL != RtlGetVersion) {
        osInfo.dwOSVersionInfoSize = sizeof(osInfo);
        RtlGetVersion(&osInfo);
        result = "Windows ";
        if(osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) result += "NT ";
        result += QString::number(osInfo.dwMajorVersion) + "." + QString::number(osInfo.dwMinorVersion);
    }
    return result;
}
#endif

QString GetOS() {
    QString OS;
#ifdef Q_OS_LINUX
    OS = "X11; Linux";
#endif
#ifdef Q_OS_FREEBSD
    OS = "X11; FreeBSD";
#endif
#ifdef Q_OS_WIN
    OS = GetWinVer();
#endif
#ifdef Q_OS_MAC
    OS = "Macintosh; Mac OSX";
#endif
    return OS;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setMaximumSize(763,547);
    this->setWindowTitle("UDG Plop [Untitled]");
    IsSaved = true;
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

    btnAbout = new QToolButton(this);
    btnAbout->setText("About");
    connect(btnAbout,SIGNAL(clicked()),this,SLOT(AboutClick()));
    ui->mainToolBar->addWidget(btnAbout);

    UpdateTextView();
    QTimer::singleShot(200, this, SLOT(CheckUpdates()));
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
    output += "\n\n";
    for(c=0;c<8;c++) {
        output += "POKE USR \"A\"+"+QString::number(c)+", BIN ";
        for(i=0;i<8;i++) {
            j = (pixels[c] >> i) & 1;
            output += QString::number(j);
        }
        output += "\n";
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

void MainWindow::AboutClick() {
    QString html;
    html = "<p><b style=\"font-size: 14pt\">"+QString(APPNAME)+"</b> "+QString(APPVER)+"<br>\n";
    html.append("&copy;2019 <a href=\"https://www.matthewhipkin.co.uk\" style=\"color: #FF0000\">Matthew Hipkin</a><br>\n");
    html.append("<p>ZX Spectrum UDG Creator.</p>");
    //html.append("<p><a href=\"https://twitter.com/hippy2094\"><img src=\":/images/logo_twitter_25px.png\"></a> <a href=\"https://sourceforge.net/projects/base64-binary/\"><img src=\":/images/sourceforge_logo_small.png\"></a> <a href=\"https://qt.io\"><img src=\":/images/Built_with_Qt_RGB_logo.png\"></p>");
    //html.append("<p><a href=\"https://qt.io\"><img src=\":/images/Built_with_Qt_RGB_logo.png\"></p>");
    QMessageBox::about(this,"About "+QString(APPNAME),html);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(!IsSaved) {
        if(QMessageBox(QMessageBox::Information, "Unsaved file", "Do you wish to save this file?", QMessageBox::Yes|QMessageBox::No).exec() == QMessageBox::Yes) {
            SaveFileClick();
        }
    }
}

void MainWindow::CheckUpdates() {
    QString ua;
    ua = "Mozilla/5.0 (compatible; "+GetOS()+"; "+APPNAME+" "+APPVER+" ("+QString::number(CURRVER)+"))";
    QNetworkAccessManager nam;
    QUrl url("http://www.matthewhipkin.co.uk/udgplop.txt");
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent",QByteArray(ua.toStdString().c_str()));
    QNetworkReply* reply = nam.get(req);
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QByteArray bytes = reply->readAll();
    QString t = QString::fromUtf8(bytes);
    if(t.trimmed().toInt() > CURRVER) {
        labelUpdate = new QLabel(this);
        labelUpdate->setText("<p>A new version is available, <a href=\"https://www.matthewhipkin.co.uk\">click here</a> to get it!");
        labelUpdate->setVisible(true);
        labelUpdate->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(255, 204, 204, 255), stop:1 rgba(255, 255, 255, 255)); }");
        labelUpdate->setTextInteractionFlags(Qt::TextBrowserInteraction);
        labelUpdate->setOpenExternalLinks(true);
        ui->statusBar->addWidget(labelUpdate);
    }
}
