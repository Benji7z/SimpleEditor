#!/bin/bash
set -e

if [ -f "README.md" ]; then
    cat README.md
    echo ""
    read -p "Press Enter to continue..."
fi


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
    echo "Added ~/bin to PATH — restart your shell or run: source ~/.bashrc"
fi

echo "Install finished, run via simpledit <file>"
echo "Oopsies! simpledit seems to not be sourced so restart your shell or run: source ~/.bashrc"