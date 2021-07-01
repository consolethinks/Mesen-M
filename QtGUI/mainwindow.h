#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../InteropDLL/ConsoleWrapper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void on_actionOpen_ROM_triggered();

private:
    Ui::MainWindow *ui;

    bool overrideWindowSize = false;
    bool audioDisable = false;
    bool videoDisable = false;
    bool inputDisable = false;
};
#endif // MAINWINDOW_H
