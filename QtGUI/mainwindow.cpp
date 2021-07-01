#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QCoreApplication>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!InteropDll::TestDll()) // Test if emu core lib is loaded
    {
        exit(-1);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    // TODO: LanguageConfig

    InteropDll::InitDll();
    // TODO: NotificationListener
    ScreenSize originalSize, newSize;
    InteropDll::GetScreenSize(ConsoleId::Master, originalSize, false);
    // TODO: VideoConfig
    InteropDll::GetScreenSize(ConsoleId::Master, newSize, false);

    if(originalSize.Width != newSize.Width || originalSize.Height != newSize.Height)
    {
        overrideWindowSize = true;
    }

    // TODO: ConfigManager

    InteropDll::InitializeEmu(".", nullptr, (void*)centralWidget()->winId(), audioDisable, videoDisable, inputDisable);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // TODO: save config, close sub-windows

    InteropDll::Stop();
    InteropDll::Release();

    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionOpen_ROM_triggered()
{
    QFileDialog dialog(this);

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Roms (*.nes *.fds)"));
    dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (dialog.exec()) fileNames = dialog.selectedFiles();
    else return;

    // TODO: make it safer
    std::string fileString = fileNames.at(0).toStdString();
    char fileCharString[fileString.length() + 1];
    char empty[1] = {'\0'};
    strncpy(fileCharString, fileString.c_str(), fileString.length() + 1);

    InteropDll::LoadROM(fileCharString, empty);
}
