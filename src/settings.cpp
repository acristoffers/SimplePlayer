#include "src/settings.h"
#include "ui_settings.h"

#include <QDebug>

#include <QFileDialog>
#include <QSettings>
#include <QStringList>

#include "database.h"
#include "librarymanager.h"

struct SettingsPrivate
{
    QSettings    settings;
    QStringList  paths;
    Ui::Settings *ui;

    void updatePathsWidget();
};

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    d(new SettingsPrivate)
{
    d->ui = new Ui::Settings;
    d->ui->setupUi(this);
    d->updatePathsWidget();

    connect( this, SIGNAL( finished(int) ),     this,                       SIGNAL( settingsChanged() ) );
    connect( this, SIGNAL( settingsChanged() ), LibraryManager::instance(), SLOT( scan() ) );
    connect( this, SIGNAL( settingsChanged() ), LibraryManager::instance(), SLOT( changeWatchPaths() ) );
}

Settings::~Settings()
{
    delete d->ui;
    delete d;
}

bool Settings::hasPath()
{
    QStringList paths = d->settings.value("paths").toStringList();

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

    QStringList paths = d->settings.value("paths").toStringList();

    paths.append(path);
    d->settings.setValue("paths", paths);

    d->updatePathsWidget();
}

void Settings::on_removePath_clicked()
{
    QList<QListWidgetItem *> items = d->ui->paths->selectedItems();

    if ( items.empty() ) {
        return;
    }

    QString path = items.first()->text();

    QStringList paths = d->settings.value("paths").toStringList();
    QStringList remaining;

    for ( auto p : paths ) {
        if ( p != path ) {
            remaining.append(p);
        }
    }

    d->settings.setValue("paths", remaining);

    d->updatePathsWidget();
}

void SettingsPrivate::updatePathsWidget()
{
    ui->paths->clear();

    QStringList paths = settings.value("paths").toStringList();

    ui->paths->addItems(paths);
}
