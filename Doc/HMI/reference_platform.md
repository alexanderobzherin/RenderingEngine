# System Identity

```
hostnamectl
cat /etc/os-release
uname -a
dpkg --print-architecture
```

---

- Hostname: `hmi-target`
- Operating system: Debian GNU/Linux 13.6 (`trixie`)
- Kernel: Linux `6.12.95+deb13-amd64`
- Architecture: `x86-64` / Debian `amd64`
- Hardware vendor reported by firmware: AZW
- Hardware model reported by firmware: MINI S
- Commercial product: Beelink Mini S
- Firmware version: `MS2V001`
- Firmware date: 2025-04-08

******************************

# Hardware Inventory – CPU

```
lscpu
grep -m1 'model name' /proc/cpuinfo
nproc
```

---

Processor
Model: Intel Processor N150
Architecture: x86-64
Physical cores: 4
Logical processors: 4
Operating frequency: 700 MHz – 3.6 GHz
Hardware virtualization: Intel VT-x
Cache hierarchy:
L1: 128 KiB data / 256 KiB instruction
L2: 2 MiB
L3: 6 MiB

********************************

# Hardware Inventory – Memory

```
free -h
grep MemTotal /proc/meminfo
dmidecode -t memory
```

---

Installed memory: 16 GB DDR4
Form factor: SODIMM
Configured speed: 3200 MT/s
Installed memory modules: 1 × 16 GB DDR4 SO-DIMM
Maximum supported memory: 32 GB
Runtime memory after fresh boot:
Used: ~500 MB
Available: ~14 GB
Swap partition: 16 GB

********************************

# Hardware Inventory – Storage

```
lsblk -o NAME,SIZE,TYPE,FSTYPE,MOUNTPOINT,MODEL
df -h
```

---

Installed storage: 512 GB SSD
Boot mode: UEFI
Partition layout:
EFI System Partition: ~1 GB (FAT32)
Root filesystem: ~460 GB (ext4)
Partition table: GPT
Swap: ~16 GB
Fresh Debian installation occupies approximately 1.4 GB, leaving approximately 428 GB available for applications, assets and logs.

The platform intentionally uses a simple GPT partition layout without LVM or filesystem encryption to facilitate development and deployment.

********************************

# Hardware Inventory – Graphics

```
lspci
lspci | grep -Ei 'vga|display|3d'
lsmod | grep i915
```

---

- GPU: Integrated Intel Alder Lake-N graphics
- PCI address: `00:02.0`
- Kernel driver: `i915`
- Kernel graphics framework: DRM/KMS
- Display output is functional before installation of Xorg, Wayland or Vulkan userspace components.

The minimal Debian installation already includes the kernel-side Intel graphics stack. Xorg, Mesa Vulkan, GLFW and Rendering Engine runtime components have not yet been installed.

### Storage Controller

- Controller: Intel Alder Lake-N SATA AHCI
- Linux storage device: `/dev/sda`

### Ethernet

- Two Intel I226-V Ethernet controllers are present.

********************************

# Hardware Inventory – Network Adapters

```
ip -br link
lspci -k | grep -A3 -Ei 'network|ethernet'
```

---

- Wi-Fi: Intel Wi-Fi 6 AX101NGW
  - Linux interface: `wlo1`
  - Kernel driver: `iwlwifi`
  - Status during commissioning: active

- Ethernet:
  - Two Intel I226-V controllers
  - Linux interfaces: `enp1s0`, `enp2s0`
  - Kernel driver: `igc`
  - Status during commissioning: inactive, no cable connected

The minimal Debian installation includes kernel support for all detected network adapters. The `iw` userspace diagnostic utility is not installed in the baseline system.

*****************************

# Hardware Inventory – Input Devices

```
lsusb
cat /proc/bus/input/devices
dmesg | grep -i -E "usb|hid|ilitek"
```

---

# USB Controllers
The Linux xHCI driver exposes four USB root hubs corresponding to the USB controllers integrated into the Intel platform.

# Touchscreen
Vendor: 222A
Product: 0001
Manufacturer: ILI Technology Corp.
Device: Multi-Touch Screen

Detected by the USB subsystem (`lsusb`).
Linux input subsystem creates two logical devices:
ILITEK ILITEK-TP
and
ILITEK ILITEK-TP Mouse
This indicates that the touchscreen is recognized as both
a native multitouch device
a mouse-compatible HID device
allowing legacy software to receive mouse events while modern software can consume absolute touch events.

# Bluetooth
Intel AX201 Bluetooth

*****************************

# Software Inventory

The complete software inventory of the reference platform was captured immediately after the initial installation of Debian GNU/Linux 13.6 and before installing any additional graphics libraries, development tools or Rendering Engine components.

The attached package list serves as the software baseline for future platform bring-up and deployment.

Attachment:
- `debian13-baseline-packages.txt`

*****************************

# Runtime Services

```
systemctl --type=service --state=running
```

---

Running services after initial Debian installation

```
Service	                    | Purpose
-------------------------------------------------------------
  cron.service              | Scheduled task execution
  dbus.service              | System message bus
  getty@tty1.service        | Local console login
  ssh.service               | Remote administration (OpenSSH)
  systemd-journald.service  | System logging
  systemd-logind.service    | User session management
  systemd-timesyncd.service | Time synchronization
  systemd-udevd.service     | Device discovery and management
  user@1000.service         | User session manager
  wpa_supplicant.service    | Wi-Fi connectivity
```

Summary

The baseline Debian installation starts only a small set of essential services required for:

system initialization and logging;
local and remote user access;
automatic hardware detection;
wireless networking;
scheduled task execution;
clock synchronization.

No graphical display manager, desktop environment services, graphics middleware, or Rendering Engine components are active.


************************

# Boot Environment

```
efibootmgr
mount | grep boot
lsblk -f
```

---

Boot Configuration
Firmware boot mode: UEFI
Boot manager: GRUB EFI
Default boot entry: Debian
EFI executable: \EFI\debian\shimx64.efi
Boot chain initiated through `shimx64.efi`, providing compatibility with UEFI Secure Boot.

Partition Layout

```
Partition | Filesystem | Purpose
/dev/sda1 | FAT32      | EFI System Partition
/dev/sda2 | ext4       | Root filesystem
/dev/sda3 | swap       | Swap space
```

Filesystem Mounts
EFI System Partition mounted at /boot/efi
Root filesystem mounted at /

Summary

The reference platform uses a standard UEFI boot configuration with a GPT partition table. The operating system is installed on a single ext4 root filesystem with a dedicated FAT32 EFI System Partition and a separate swap partition. This simple layout facilitates system maintenance, deployment and recovery.

## Network Configuration

The HMI target is configured to obtain its IPv4 address via DHCP.

To provide a stable development environment while retaining DHCP flexibility,
the home router contains a static DHCP reservation mapping the wireless MAC
address to a fixed IP address.

| Property | Value |
|----------|-------|
| Hostname | hmi-target |
| Interface | wlo1 |
| MAC Address | 98:FE:3E:08:D5:F9 |
| Reserved IPv4 | 192.168.1.154 |
| Gateway | 192.168.1.1 |

This approach preserves automatic gateway/DNS configuration while ensuring a
stable address for SSH access, deployment scripts and development tooling.