#!/bin/bash

gcc cobra_hash.c -o cobra_hash -lssl -lcrypto

if [ $? -eq 0 ]; then
  echo "Build successful: cobra_hash"
else
  echo "Build failed."
  exit 1
fi
