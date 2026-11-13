# KCM Cloud Mouse

<p align="center">
  <strong>KDE System Settings module for mouse configuration in Cloud Desktop</strong><br/>
  <em>Scroll factor · Pointer speed · Acceleration · D-Bus powered</em>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/KDE-Plasma%206-blue?logo=kde" />
  <img src="https://img.shields.io/badge/Qt-6-green?logo=qt" />
  <img src="https://img.shields.io/badge/Gamescope-Cloud%20Desktop-orange" />
  <img src="https://img.shields.io/badge/license-GPL--2.0-lightgrey" />
</p>

---

## Что это

KDE System Settings модуль (KCM), предоставляющий графический интерфейс для настройки мыши в окружении [Cloud Desktop](https://github.com/wolfam0108/Cloud-Desktop) (Gamescope + KWin + Sunshine).

В Cloud Desktop мышь проходит нестандартный путь: **Moonlight → Sunshine → uinput → libinput (Gamescope)** — KDE Plasma не видит физическое устройство и стандартный KCM `kcm_mouse` не работает. Этот модуль общается напрямую с Gamescope через D-Bus, минуя стандартный KDE input stack.

---

## Зачем нужен

В обычном KDE Plasma настройки мыши управляются через `libinput` → KWin → `kcm_mouse`. В Cloud Desktop цепочка другая:

```
Стандартный KDE:
  Физическая мышь → libinput → KWin → kcm_mouse (System Settings)

Cloud Desktop:
  Moonlight → Sunshine → uinput → libinput → Gamescope (CLibInputHandler)
                                                  ↑
                                         kcm_cloud_mouse (этот модуль)
                                         общается через D-Bus
```

KWin в Cloud Desktop — **вложенный** композитор, он не имеет доступа к libinput устройствам. Gamescope обрабатывает ввод в отдельном потоке (`CLibInputHandler`) и экспортирует настройки через D-Bus интерфейс `org.gamescope.Input`.

---

## Возможности

| Настройка | Описание | Диапазон |
|---|---|---|
| **Scroll Factor** | Множитель скорости прокрутки | 0.1 — 20.0 (по умолчанию 8.0) |
| **Pointer Speed** | Скорость указателя (libinput acceleration speed) | -1.0 — 1.0 |
| **Acceleration Profile** | Профиль ускорения: Flat (линейный) или Adaptive | flat / adaptive |
| **Natural Scrolling** | Инвертированная прокрутка (как на тачпаде) | on / off |
| **Left-Handed Mode** | Поменять кнопки мыши местами | on / off |
| **Middle Button Emulation** | Эмуляция средней кнопки (ЛКМ+ПКМ одновременно) | on / off |
| **Enable/Disable Device** | Включить/выключить устройство | on / off |

Все настройки **персистентны** — сохраняются в файл и восстанавливаются при перезапуске Gamescope.

---

## Архитектура

```
┌───────────────────────────┐
│  KDE System Settings      │
│  (Input Devices →         │
│   Cloud Mouse)            │
│                           │
│  ┌─────────────────────┐  │
│  │  main.qml (UI)      │  │
│  │  SpinBox, Slider,   │  │
│  │  RadioButton, Check │  │
│  └──────┬──────────────┘  │
│         │ Q_PROPERTY      │
│  ┌──────▼──────────────┐  │
│  │  CloudMouseDevice   │  │
│  │  (C++ backend)      │  │
│  └──────┬──────────────┘  │
│         │ D-Bus           │
└─────────┼─────────────────┘
          │
          ▼
┌──────────────────────────┐
│  Gamescope               │
│  org.gamescope.Input     │
│  ┌────────────────────┐  │
│  │ CLibInputHandler   │  │
│  │ (gamescope-libinput │  │
│  │  thread, 1000 Hz)  │  │
│  └────────────────────┘  │
└──────────────────────────┘
```

### Компоненты

| Файл | Роль |
|---|---|
| `src/kcm_cloud_mouse.cpp` | KCM плагин — точка входа, регистрация модуля |
| `src/cloud_mouse_backend.cpp/h` | Backend — обнаружение устройств через D-Bus, управление жизненным циклом |
| `src/cloud_mouse_device.cpp/h` | Device proxy — чтение/запись свойств устройства через D-Bus (`org.gamescope.InputDevice`) |
| `src/qml/main.qml` | UI — QML интерфейс с контролами для каждого свойства |
| `kcm_cloud_mouse.desktop` | Desktop entry — интеграция в KDE System Settings |
| `resources.qrc` | Qt Resources — упаковка QML в бинарник |

### D-Bus интерфейс

Модуль взаимодействует с Gamescope через D-Bus:

| Сервис | Путь | Интерфейс |
|---|---|---|
| `org.gamescope.Input` | `/org/gamescope/InputDevice/<sysname>` | `org.gamescope.InputDevice` |

Свойства D-Bus:

| Property | Type | R/W | Описание |
|---|---|---|---|
| `Name` | string | R | Имя устройства (напр. "Sunshine Mouse") |
| `Enabled` | bool | RW | Включено ли устройство |
| `PointerAcceleration` | double | RW | Скорость указателя (-1.0 .. 1.0) |
| `AccelProfileFlat` | bool | RW | Линейный профиль ускорения |
| `AccelProfileAdaptive` | bool | RW | Адаптивный профиль ускорения |
| `NaturalScroll` | bool | RW | Естественная прокрутка |
| `ScrollFactor` | double | RW | Множитель прокрутки |
| `LeftHanded` | bool | RW | Для левшей |
| `MiddleEmulation` | bool | RW | Эмуляция средней кнопки |
| `SupportsPointerAcceleration` | bool | R | Поддерживает ли устройство ускорение |
| `SupportsNaturalScroll` | bool | R | Поддерживает ли естественную прокрутку |
| `SupportsLeftHanded` | bool | R | Поддерживает ли режим для левшей |
| `SupportsMiddleEmulation` | bool | R | Поддерживает ли эмуляцию средней |

---

## Зависимости для сборки

```bash
# Fedora 43, 44, 45
sudo dnf install cmake gcc-c++ extra-cmake-modules \
    qt6-qtbase-devel qt6-qtdeclarative-devel \
    kf6-kcmutils-devel kf6-ki18n-devel kf6-kcoreaddons-devel
```

---

## Установка из релиза (Fedora 43, 44, 45)

```bash
sudo dnf install https://github.com/chiririll/kcm-cloud-mouse/releases/latest/download/kcm-cloud-mouse-fc$(rpm -E %fedora)-x86_64.rpm
```

RPM для каждой версии Fedora собирается отдельно GitHub Actions ([.github/workflows/release.yml](.github/workflows/release.yml)): тег `vX.Y.Z` — сборка и релиз.

## Сборка и установка из исходников

```bash
git clone https://github.com/chiririll/kcm-cloud-mouse.git
cd kcm-cloud-mouse
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build -j$(nproc)
sudo cmake --install build
```

Или RPM локально (в контейнере Fedora):

```bash
docker run --rm -v "$PWD:/src" -w /src fedora:43 packaging/build-rpm.sh 1.0.1   # → out/*.rpm
```

После установки модуль доступен в **System Settings → Input Devices → Cloud Mouse**.

---

## Требования

- **Gamescope** с Sunshine backend ([Cloud Desktop](https://github.com/wolfam0108/Cloud-Desktop)) — экспортирует D-Bus интерфейс `org.gamescope.Input`
- **KDE Plasma 6** / KDE Frameworks 6
- **Qt 6** (Core, DBus, Qml, Quick)

---

## Лицензия

[GPL-2.0-or-later](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)
