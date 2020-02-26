#!/bin/sh

# Copy PKGBUILD for given Architecture
cp PKGBUILD_$1 PKGBUILD

# Make Package
makepkg -f

# Remove PKGBUILD for given Architecture
rm PKGBUILD
