/*
    Cloud Mouse Backend — обнаружение устройств через D-Bus
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "cloud_mouse_device.h"

#include <QObject>
#include <QList>
#include <QDBusInterface>
#include <memory>
#include <vector>

class CloudMouseBackend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<CloudMouseDevice*> devices READ devices NOTIFY devicesChanged)
    Q_PROPERTY(int deviceCount READ deviceCount NOTIFY devicesChanged)
    Q_PROPERTY(QString errorString READ errorString CONSTANT)

public:
    explicit CloudMouseBackend(QObject *parent = nullptr);

    bool load();
    bool save();
    bool defaults();
    bool isSaveNeeded() const;
    QString errorString() const { return m_errorString; }

    QList<CloudMouseDevice*> devices() const;
    int deviceCount() const { return m_devices.size(); }

Q_SIGNALS:
    void devicesChanged();
    void needsSaveChanged();

private Q_SLOTS:
    void onDeviceAdded(const QString &sysName);
    void onDeviceRemoved(const QString &sysName);

private:
    void findDevices();

    std::vector<std::unique_ptr<CloudMouseDevice>> m_devices;
    QDBusInterface *m_manager = nullptr;
    QString m_errorString;
};
