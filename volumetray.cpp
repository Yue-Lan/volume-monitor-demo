#include "volumetray.h"
#include <QDebug>
#include <QApplication>
#include <QTimer>

static void unmount_callback (GMount* mount, VolumeTray* volumeTray);

static void show_mount_root_directory (GMount *mount);

VolumeTray::VolumeTray()
{

}

VolumeTray::~VolumeTray() {
    delete mountMenu;
}

void VolumeTray::onActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == DoubleClick && mMount != NULL) {
        show_mount_root_directory (mMount);
    }
}

void VolumeTray::setMount(GVolumeMonitor *, GMount *mount) {
    mMount = mount;

    g_signal_connect (mount, "unmounted", G_CALLBACK (unmount_callback), this);
    mountMenu = new QMenu;

    QAction *openAction = new QAction(mountMenu);
    openAction->setText("show");
    mountMenu->addAction(openAction);
    connect(openAction, &QAction::triggered, [=](){
        show_mount_root_directory (mount);
    });

    QAction *ejectAction = new QAction(mountMenu);
    ejectAction->setText("eject");
    mountMenu->addAction(ejectAction);
    connect(ejectAction, &QAction::triggered, [mount](){
        if (g_mount_can_eject (mount)) {
            g_mount_eject_with_operation(mount, G_MOUNT_UNMOUNT_NONE, NULL, NULL, NULL, NULL);
        }
    });

    QAction *unmountAction = new QAction(mountMenu);
    unmountAction->setText("unmount");
    mountMenu->addAction(unmountAction);
    connect(unmountAction, &QAction::triggered, [mount](){
        if (g_mount_can_unmount (mount)) {
            g_mount_unmount_with_operation(mount, G_MOUNT_UNMOUNT_NONE, NULL, NULL, NULL, NULL);
        }
    });

    mountMenu->addSeparator();

    QAction *quitAppAction = new QAction(mountMenu);
    quitAppAction->setText("quit all");
    quitAppAction->setToolTip("quit the whole application, never monitor the volume yet");
    mountMenu->addAction(quitAppAction);
    quitAppAction->setWhatsThis("quit the whole application, never monitor the volume yet");
    connect(quitAppAction, &QAction::triggered, qApp, &QApplication::quit);

    this->setContextMenu(mountMenu);
}

void unmount_callback (GMount *mount, VolumeTray *volumeTray) {
    qDebug()<<"unmount_callback";
    g_signal_handlers_disconnect_by_func (mount, (gpointer) unmount_callback, NULL);
    char *mount_name = g_mount_get_name(mount);
    volumeTray->showMessage("unmounted", QString(mount_name), QSystemTrayIcon::Information, 1000);
    //TODO: is there any way just showing messagebox without the tray icon?
    //volumeTray->hide();
    //volumeTray->setVisible(false);
    QTimer *timer = new QTimer(volumeTray);
    timer->start(1000);
    QObject::connect(timer, &QTimer::timeout, [volumeTray](){
        qDebug()<<"time out";
        volumeTray->~VolumeTray();
    });
}

void show_mount_root_directory (GMount *mount) {
    GFile *tmp_file = g_mount_get_root (mount);
    char *tmp_uri = g_file_get_uri(tmp_file);
    g_app_info_launch_default_for_uri(tmp_uri, NULL, NULL);
    g_free (tmp_uri);
    g_object_unref(tmp_file);
}
