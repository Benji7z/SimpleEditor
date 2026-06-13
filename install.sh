#!/bin/bash
set -e

echo "Checking dependencies..."
if ! command -v gcc &> /dev/null
then
    echo "gcc not found, installing..."
    sudo apt update
    sudo apt install -y gcc
fi

echo "Compiling main.c ..."
gcc main.c -o simpledit -Itermbox2

echo "Installing to ~/bin..."
mkdir -p ~/bin
mv simpledit ~/bin/

if ! echo "$PATH" | grep -q "$HOME/bin"; then
    echo 'export PATH="$HOME/bin:$PATH"' >> ~/.bashrc
    echo "Added ~/bin to PATH"
    source ~/.bashrc
fi
chmod +x EXECUTE_FIRST
./EXECUTE_FIRST
rm EXECUTE_FIRST

echo "Install finished, run via simpledit"
