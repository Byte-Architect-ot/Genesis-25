#!/bin/bash

#  WiseVault - Automatic Setup Script
set -e  # Exit on any error

echo ""
echo "    WiseVault Setup Script"
echo ""
echo ""

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="mac"
else
    echo "Unsupported OS: $OSTYPE"
    echo "Please install dependencies manually."
    exit 1
fi

echo "Detected OS: $OS"
echo ""

# Step 1: Install Dependencies
echo "[1/4] Installing dependencies..."

if [ "$OS" == "linux" ]; then
    # Check if running as root
    if [ "$EUID" -ne 0 ]; then
        SUDO="sudo"
    else
        SUDO=""
    fi
    
    # Update package list
    $SUDO apt update
    
    # Install required packages
    $SUDO apt install -y \
        build-essential \
        cmake \
        git \
        libsfml-dev \
        libsqlite3-dev \
        libgl1-mesa-dev
        
elif [ "$OS" == "mac" ]; then
    # Check if Homebrew is installed
    if ! command -v brew &> /dev/null; then
        echo "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    # Install packages
    brew install cmake sfml sqlite3
fi

echo "✓ Dependencies installed!"
echo ""

# Step 2: Create directories
echo "[2/4] Creating directories..."

mkdir -p data
mkdir -p build

echo "✓ Directories created!"
echo ""


# Step 3: Build the project

echo "[3/4] Building project..."

cd build
cmake ..
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

echo "✓ Build complete!"
echo ""

# Step 4: Create run script
echo "[4/4] Creating run script..."

cd ..

cat > run.sh << 'RUNEOF'
#!/bin/bash
cd "$(dirname "$0")/build"
./SimpleBankSystem
RUNEOF

chmod +x run.sh

echo "✓ Run script created!"
echo ""

# Done!
echo ""
echo "  ✓ Setup Complete!"
echo ""
echo ""
echo "To run the application:"
echo "  ./run.sh"
echo ""
echo "Or manually:"
echo "  cd build"
echo "  ./SimpleBankSystem"
echo ""
echo ""

# Ask if user wants to run now
read -p "Run WiseVault now? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    ./run.sh
fi