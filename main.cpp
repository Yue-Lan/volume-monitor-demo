#include "volumetray.h"

#include <QApplication>
#include <gio/gio.h>
#include <glib.h>

#include <QIcon>

#include <QDebug>

static void drive_connected_callback (GVolumeMonitor *monitor, GDrive *drive, gpointer);
static void drive_disconnected_callback (GVolumeMonitor *monitor, GDrive *drive, gpointer);
static void volume_added_callback (GVolumeMonitor *monitor, GVolume *volume, gpointer);
static void volume_removed_callback (GVolumeMonitor *monitor, GVolume *volume, gpointer);
static void mount_added_callback (GVolumeMonitor *monitor, GMount *mount, gpointer);
static void mount_removed_callback (GVolumeMonitor *, GMount *mount, gpointer);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QIcon::setThemeName("ukui-icon-theme");

    GVolumeMonitor *monitor = g_volume_monitor_get();
    g_signal_connect (monitor, "drive-connected", G_CALLBACK (drive_connected_callback), NULL);
    g_signal_connect (monitor, "drive-disconnected", G_CALLBACK (drive_disconnected_callback), NULL);
    g_signal_connect (monitor, "volume-added", G_CALLBACK (volume_added_callback), NULL);
    g_signal_connect (monitor, "volume-removed", G_CALLBACK (volume_removed_callback), NULL);
    g_signal_connect (monitor, "mount-added", G_CALLBACK (mount_added_callback), NULL);
    g_signal_connect (monitor, "mount-removed", G_CALLBACK (mount_removed_callback), NULL);

    return a.exec();
}

void drive_connected_callback (GVolumeMonitor *, GDrive *drive, gpointer) {
    qDebug()<<"drive connected";
    char *drive_name = g_drive_get_name(drive);
    qDebug()<<"name: "<<drive_name;
    if (g_drive_can_eject (drive)) {
        qDebug()<<"edjectable drive";
    }
}

void drive_disconnected_callback (GVolumeMonitor *, GDrive *drive, gpointer) {
    qDebug()<<"drive disconnected";
    char *drive_name = g_drive_get_name(drive);
    qDebug()<<"name: "<<drive_name;
}

void volume_added_callback (GVolumeMonitor *, GVolume *volume, gpointer) {
    qDebug()<<"volume added";
    char *volume_name = g_volume_get_name(volume);
    qDebug()<<"name: "<<volume_name;
    if (g_volume_can_eject (volume)) {
        qDebug()<<"ejectable volume";
    }
}

void volume_removed_callback (GVolumeMonitor *, GVolume *volume, gpointer) {
    qDebug()<<"volume removed";
    char *volume_name = g_volume_get_name(volume);
    qDebug()<<"name: "<<volume_name;
}

void mount_added_callback (GVolumeMonitor *monitor, GMount *mount, gpointer) {
    qDebug()<<"mount added";
    char *mount_name = g_mount_get_name(mount);
    qDebug()<<"name: "<<mount_name;
    if (g_mount_can_eject (mount)) {
        qDebug()<<"ejectable mount";
        VolumeTray *tray = new VolumeTray;

        tray->setMount(monitor, mount);
        GIcon *tmp_icon = g_mount_get_icon(mount);
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (tmp_icon));
        qDebug()<<*icon_names;
        tray->setIcon (QIcon::fromTheme(*icon_names));
        g_object_unref (tmp_icon);
        tray->show();
        char* mount_name = g_mount_get_name (mount);
        tray->showMessage(QString ("mount added"), QString (mount_name), QIcon::fromTheme(*icon_names), 5000);

        QObject::connect(tray, &VolumeTray::messageClicked, [mount](){
            GFile *tmp_file = g_mount_get_root (mount);
            char *tmp_uri = g_file_get_uri(tmp_file);
            g_app_info_launch_default_for_uri(tmp_uri, NULL, NULL);
            g_free (tmp_uri);
            g_object_unref(tmp_file);
        });

        QObject::connect(tray, &VolumeTray::activated, tray, &VolumeTray::onActivated);

        g_free (mount_name);
    }
}

void mount_removed_callback (GVolumeMonitor *, GMount *mount, gpointer) {
    qDebug()<<"mount removed";
    char *mount_name = g_mount_get_name(mount);
    qDebug()<<"name: "<<mount_name;
}
