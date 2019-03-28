#ifndef VOLUMETRAY_H
#define VOLUMETRAY_H
#include <QSystemTrayIcon>

#include <gio/gio.h>
#include <glib.h>

#include <QMenu>

class VolumeTray : public QSystemTrayIcon
{
public:
    VolumeTray();
    ~VolumeTray ();

    void setMount (GVolumeMonitor *monitor, GMount* mount);
    void setVolume (GVolumeMonitor *monitor, GVolume* volume);
    void setDrive (GVolumeMonitor *monitor, GDrive *drive);

public Q_SLOTS:
    void onActivated (QSystemTrayIcon::ActivationReason reason);

private:
    QMenu *mountMenu;
    GMount *mMount;
};

#endif // VOLUMETRAY_H
