# pulseaudio-led-mute

`pulseaudio-led-mute` runs in the background and synchronizes mute status of PulseAudio with LED indicators.

LED indication of audio mute status is a common case for IBM/Lenovo ThinkPad or Huawei laptops.

Hardware audio devices mute status indications is managed by ALSA. However in contemporary Linux systems ALSA is not being directly used by client applications. Rather there is an intermediate sound server like PulseAudio or PipeWire.

PulseAudio or PipeWire do not manage LED mute indication themselves, which leaves virtual audio devices without proper indication.

`pulseaudio-led-mute` receives mute status from PulseAudio server, and updates the corresponding LED indicators. This way the indication is consistent with the actual playback or recording devices being in use, whether it is Bluetooth, USB, or a virtual device.

`pulseaudio-led-mute` supports native PulseAudio server or the one implemented by PipeWire.

## Configuration

The service is being registered as a `systemd` user service.

After installation the following configuration files created:

- `/etc/pulseaudio-led-mute.yml`
- `/etc/udev/rules.d/99-led-audio.rules`

### LED devices ownership

In order to change LED status the service must be able to access LED device. In Linux LED devices are owned by user `root`. Therefore an `udev` rule is required to change the device owner. The supported devices are listed in `/etc/udev/rules.d/99-led-audio-mute.rules`.

It is expected the user running the service is a member of `audio` group, and the ownership of LED audio devices is set correspondingly.

After the installation it is required to restart `udev` service for the new rules being applied.

### Device mapping

The mappings between audio devices and LED indicators are configured in `pulseaudio-led-mute.yml`.
It is possible to specify exact names of audio devices, or bind to the default audio sink and source.

Multiple mappings may be configured to handle different LED indicators. In order to use the additional LED indicators the `udev` rule must be adjusted accordingly.

## Installation

There is a binary DEB package for the latest Ubuntu release under `Releases`.

Alternatively the program may be compiled from the source as below.

Install build dependencies:

```sh
apt update
apt install build-essential cmake file gcc libboost-filesystem-dev libboost-program-options-dev libboost-system-dev libpulse-dev libyaml-cpp-dev scdoc
```

Configure and build:

```sh
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
cmake --build .
```

Install:

```sh
cmake --install .
```

Alternatively, instead of raw installation a Debian package may be created:

```sh
cpack
```

## Legal

The author is not associated with freedesktop.org project.

All trademarks belong to their respective owners.

Additional terms are specified in `LICENSE` file.
