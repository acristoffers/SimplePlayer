#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

struct SettingsPrivate;

class Settings : public QDialog
{
    Q_OBJECT

public:
    Settings(QWidget *parent = 0);
    ~Settings();

    bool hasPath();

    void requestToAddPath();

private slots:
    void on_addPath_clicked();
    void on_removePath_clicked();

signals:
    void settingsChanged();

private:
    SettingsPrivate *d;
};
#endif // SETTINGS_H
