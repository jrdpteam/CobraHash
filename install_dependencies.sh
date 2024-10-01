#!/bin/bash

if ! dpkg -s libssl-dev >/dev/null 2>&1; then
  echo "Installing OpenSSL development libraries..."
  sudo apt-get update
  sudo apt-get install -y libssl-dev
else
  echo "OpenSSL development libraries already installed."
fi
