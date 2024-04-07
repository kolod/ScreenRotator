# ScreenRotator
ScreenRotator

## Setup

 1. Install [vscode](https://code.visualstudio.com/download).
 2. Install [docker desktop](https://www.docker.com/products/docker-desktop/).
 3. Open root folder in vscode.
 4. In command menu select `Dev Containers: RebReopen in Container`.
 5. Select `firmware` or `frontend`.

## Setup WSL

 1. Run `wsl --list --all` in PowerShell. If docker-desktop is set as default change it to Debian/Ubuntu by `wsl --setdefault Debian` or `wsl --setdefault Ubuntu`. If linux distro not installed install it from Microsoft Store.
 2. Install [usbipd](https://github.com/dorssel/usbipd-win/releases).
 2. Run `usbipd bind --hardware-id 1a86:55d3` in PowerShell console as Administrator.
 3. Run `usbipd attach --wsl --auto-attach --hardware-id 1a86:55d3` in PowerShell console as normal user. WSL must be running.
