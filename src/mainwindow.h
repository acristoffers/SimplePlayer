#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

struct MainWindowPrivate;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_actionQuit_triggered();
    void on_actionSettings_triggered();
    void on_actionAdd_Path_triggered();

private:
    MainWindowPrivate *d;
};
#endif // MAINWINDOW_H
