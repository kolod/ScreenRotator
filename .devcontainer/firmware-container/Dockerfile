FROM mcr.microsoft.com/vscode/devcontainers/base:bookworm

# Install clang, python & udev
RUN sudo apt-get update
RUN export DEBIAN_FRONTEND=noninteractive
RUN sudo apt-get -y install --no-install-recommends python3-venv

## Set up udev rules for PlatformIO
RUN usermod -a -G dialout vscode
RUN usermod -a -G plugdev vscode

# Install PlatformIO CLI
USER vscode
RUN curl -fsSL -o /tmp/get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
RUN python3 /tmp/get-platformio.py
RUN echo 'export PATH="$PATH:$HOME/.platformio/penv/bin"' | tee -a /home/vscode/.bashrc
RUN echo 'export PATH="$PATH:$HOME/.platformio/penv/bin"' | sudo tee -a /root/.bashrc
