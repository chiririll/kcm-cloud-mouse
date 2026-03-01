/*
    Cloud Mouse Backend — реализация
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cloud_mouse_backend.h"
#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>

static constexpr const char *SERVICE = "org.gamescope.Input";
static constexpr const char *MANAGER_PATH = "/org/gamescope/Input";
static constexpr const char *MANAGER_IFACE = "org.gamescope.InputDeviceManager";
static constexpr const char *DEVICE_IFACE = "org.gamescope.InputDevice";

CloudMouseBackend::CloudMouseBackend(QObject *parent)
    : QObject(parent)
    , m_manager(new QDBusInterface(
          QLatin1String(SERVICE),
          QLatin1String(MANAGER_PATH),
          QLatin1String(MANAGER_IFACE),
          QDBusConnection::sessionBus(),
          this))
{
    // Connect hotplug signals
    QDBusConnection::sessionBus().connect(
        QLatin1String(SERVICE),
        QLatin1String(MANAGER_PATH),
        QLatin1String(MANAGER_IFACE),
        QStringLiteral("deviceAdded"),
        this, SLOT(onDeviceAdded(QString)));

    QDBusConnection::sessionBus().connect(
        QLatin1String(SERVICE),
        QLatin1String(MANAGER_PATH),
        QLatin1String(MANAGER_IFACE),
        QStringLiteral("deviceRemoved"),
        this, SLOT(onDeviceRemoved(QString)));

    findDevices();
}

void CloudMouseBackend::findDevices()
{
    QVariant reply = m_manager->property("devicesSysNames");
    if (!reply.isValid()) {
        m_errorString = tr("Cannot connect to Gamescope input service. "
                           "Make sure Gamescope is running with D-Bus support.");
        qWarning() << "[cloud-mouse]" << m_errorString;
        return;
    }

    QStringList sysNames = reply.toStringList();
    qDebug() << "[cloud-mouse] Found" << sysNames.size() << "devices";

    for (const QString &sn : sysNames) {
        // Check if pointer (not touchpad)
        QDBusInterface devIface(QLatin1String(SERVICE),
            QStringLiteral("/org/gamescope/Input/Device/") + sn,
            QLatin1String(DEVICE_IFACE),
            QDBusConnection::sessionBus(), this);

        QVariant isPointer = devIface.property("pointer");
        if (!isPointer.isValid() || !isPointer.toBool())
            continue;

        QVariant isTouchpad = devIface.property("touchpad");
        if (isTouchpad.isValid() && isTouchpad.toBool())
            continue;

        auto dev = std::make_unique<CloudMouseDevice>(sn, this);
        if (!dev->init()) {
            qWarning() << "[cloud-mouse] Failed to init device" << sn;
            continue;
        }

        connect(dev.get(), &CloudMouseDevice::needsSaveChanged,
                this, &CloudMouseBackend::needsSaveChanged);

        qDebug() << "[cloud-mouse] Mouse device:" << dev->name() << "(" << sn << ")";
        m_devices.push_back(std::move(dev));
    }
}

bool CloudMouseBackend::load()
{
    bool ok = true;
    for (auto &dev : m_devices)
        ok &= dev->init();
    return ok;
}

bool CloudMouseBackend::save()
{
    bool ok = true;
    for (auto &dev : m_devices)
        ok &= dev->save();
    return ok;
}

bool CloudMouseBackend::defaults()
{
    bool ok = true;
    for (auto &dev : m_devices)
        ok &= dev->defaults();
    return ok;
}

bool CloudMouseBackend::isSaveNeeded() const
{
    for (const auto &dev : m_devices) {
        if (dev->isSaveNeeded())
            return true;
    }
    return false;
}

QList<CloudMouseDevice*> CloudMouseBackend::devices() const
{
    QList<CloudMouseDevice*> list;
    list.reserve(m_devices.size());
    for (const auto &dev : m_devices)
        list.append(dev.get());
    return list;
}

void CloudMouseBackend::onDeviceAdded(const QString &sysName)
{
    // Check if already exists
    for (const auto &dev : m_devices) {
        if (dev->sysName() == sysName)
            return;
    }

    QDBusInterface devIface(QLatin1String(SERVICE),
        QStringLiteral("/org/gamescope/Input/Device/") + sysName,
        QLatin1String(DEVICE_IFACE),
        QDBusConnection::sessionBus(), this);

    QVariant isPointer = devIface.property("pointer");
    if (!isPointer.isValid() || !isPointer.toBool())
        return;
    QVariant isTouchpad = devIface.property("touchpad");
    if (isTouchpad.isValid() && isTouchpad.toBool())
        return;

    auto dev = std::make_unique<CloudMouseDevice>(sysName, this);
    if (!dev->init())
        return;

    connect(dev.get(), &CloudMouseDevice::needsSaveChanged,
            this, &CloudMouseBackend::needsSaveChanged);

    m_devices.push_back(std::move(dev));
    emit devicesChanged();
}

void CloudMouseBackend::onDeviceRemoved(const QString &sysName)
{
    auto it = std::find_if(m_devices.begin(), m_devices.end(),
        [&sysName](const std::unique_ptr<CloudMouseDevice> &dev) {
            return dev->sysName() == sysName;
        });

    if (it == m_devices.end())
        return;

    m_devices.erase(it);
    emit devicesChanged();
    emit needsSaveChanged();
}

#include "moc_cloud_mouse_backend.cpp"
