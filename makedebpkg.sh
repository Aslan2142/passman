#!/bin/sh

# Compile
mkdir build
cd build
qmake ..
make
cd ..

# Make Folders
mkdir -p build/passman_debian/DEBIAN
mkdir -p build/passman_debian/usr/bin
mkdir -p build/passman_debian/usr/share/applications
mkdir -p build/passman_debian/opt/aslan/icons

# Copy files
cp control_$1 build/passman_debian/DEBIAN/control
cp build/passman build/passman_debian/usr/bin/passman
cp passman.desktop build/passman_debian/usr/share/applications/passman.desktop
cp passman_icon.svg build/passman_debian/opt/aslan/icons/passman_icon.svg

# Make Package
dpkg-deb --build build/passman_debian
mv build/passman_debian.deb passman_debian_$1.deb
