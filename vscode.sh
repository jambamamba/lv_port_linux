#!/bin/bash
user=$(id -un)
sudo chown 0:0 /home/${user}/Downloads/VSCode-linux-x64/chrome-sandbox
sudo chmod 4755 /home/${user}/Downloads/VSCode-linux-x64/chrome-sandbox
sudo mkdir -p /home/${user}/.confg/Code
sudo chown $(id -u):$(id -g) /home/${user}/.confg/Code
sudo chown $(id -u):$(id -g) /home/${user}/.vscode/extensions
sudo chown -R $(id -u):$(id -g) /run/user/$(id -u)
/home/${user}/Downloads/VSCode-linux-x64/code --ozone-platform=wayland &
