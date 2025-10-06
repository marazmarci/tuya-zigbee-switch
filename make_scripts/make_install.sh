#!/usr/bin/env bash

# Installs dependencies and (re)downloads required sdk and toolchain.
# Run this script only once after cloning the repository.

set -e                                           # Exit on error.
cd "$(dirname "$(dirname "$(realpath "$0")")")"  # Go to project root.

sudo apt-get update
sudo apt-get install -y make python3 python3-jinja2 python3-yaml unzip wget

sudo wget https://github.com/mikefarah/yq/releases/latest/download/yq_linux_amd64 -O /usr/local/bin/yq && \
    sudo chmod +x /usr/local/bin/yq

make clean_install
make install