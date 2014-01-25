#include "settings.h"
#include "ui_settings.h"

#include <QDebug>

#include <QFileDialog>
#include <QSettings>
#include <QStringList>

#include <database.h>
#include <librarymanager.h>

struct SettingsPrivate
{
    QStringList  paths;
    Ui::Settings *ui;

    void updatePathsWidget();
};

Settings::Settings(QWidget *parent)
    : QDialog(parent),
      d(new SettingsPrivate)
{
    d->ui = new Ui::Settings;
    d->ui->setupUi(this);
    d->updatePathsWidget();

    connect( this, SIGNAL( finished(int) ), this,                       SIGNAL( settingsChanged() ) );
    connect( this, SIGNAL( finished(int) ), LibraryManager::instance(), SLOT( startScan() ) );
}

Settings::~Settings()
{
    delete d->ui;
    delete d;
}

bool Settings::hasPath()
{
    QStringList paths = LibraryManager::searchPaths();

    return !paths.empty();
}

void Settings::requestToAddPath()
{
    on_addPath_clicked();
}

void Settings::on_addPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory( 0, tr("Media directory"), QDir::homePath() );

    if ( path.isEmpty() ) {
        return;
    }

    QStringList paths = LibraryManager::searchPaths();

    paths.append(path);
    LibraryManager::instance()->setSearchPaths(paths);

    d->updatePathsWidget();
}

void Settings::on_removePath_clicked()
{
    QList<QListWidgetItem *> items = d->ui->paths->selectedItems();

    if ( items.empty() ) {
        return;
    }

    QString path = items.first()->text();

    QStringList paths = LibraryManager::searchPaths();
    QStringList remaining;

    for ( auto p : paths ) {
        if ( p != path ) {
            remaining.append(p);
        }
    }

    LibraryManager::instance()->setSearchPaths(remaining);

    d->updatePathsWidget();
}

void SettingsPrivate::updatePathsWidget()
{
    ui->paths->clear();

    QStringList paths = LibraryManager::searchPaths();

    ui->paths->addItems(paths);
}
