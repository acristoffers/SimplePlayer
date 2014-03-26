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
    static MainWindow *instance();

protected:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void about();

    void statusForProcessingFile(QString, unsigned long long, unsigned long long);

private slots:
    void on_actionQuit_triggered();
    void on_actionSettings_triggered();
    void on_actionAdd_Path_triggered();
    void on_actionRescan_triggered();

    void updateTextPosition(qint64);

private:
    MainWindowPrivate *d;
    static MainWindow *_self;
};
#endif // MAINWINDOW_H
