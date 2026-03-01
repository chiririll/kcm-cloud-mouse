/*
    KCM Cloud Mouse — KDE System Settings модуль
    Настройка мыши через Gamescope D-Bus (org.gamescope.Input)

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cloud_mouse_backend.h"
#include "cloud_mouse_device.h"

#include <KPluginFactory>
#include <KQuickConfigModule>

class KCMCloudMouse : public KQuickConfigModule
{
    Q_OBJECT
    Q_PROPERTY(CloudMouseBackend* backend READ backend CONSTANT)

public:
    explicit KCMCloudMouse(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
        , m_backend(new CloudMouseBackend(this))
    {
        setButtons(Apply | Default);

        qmlRegisterUncreatableType<CloudMouseDevice>(
            "org.kde.plasma.private.kcm_cloud_mouse", 1, 0,
            "CloudMouseDevice", QString());

        connect(m_backend, &CloudMouseBackend::needsSaveChanged,
                this, &KCMCloudMouse::onNeedsSaveChanged);
    }

    CloudMouseBackend *backend() const { return m_backend; }

public Q_SLOTS:
    void save() override
    {
        m_backend->save();
        setNeedsSave(false);
    }

    void load() override
    {
        m_backend->load();
        setNeedsSave(false);
    }

    void defaults() override
    {
        m_backend->defaults();
        setNeedsSave(m_backend->isSaveNeeded());
    }

private Q_SLOTS:
    void onNeedsSaveChanged()
    {
        setNeedsSave(m_backend->isSaveNeeded());
    }

private:
    CloudMouseBackend *m_backend;
};

K_PLUGIN_CLASS_WITH_JSON(KCMCloudMouse, "kcm_cloud_mouse.json")

#include "kcm_cloud_mouse.moc"
