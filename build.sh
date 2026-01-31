#!/bin/bash

cd ~/Project/genesis/WiseVault
mkdir -p data build
cd build
cmake ..
make -j$(nproc)

echo ""
echo "✓ Build complete! Run with: ./run.sh"