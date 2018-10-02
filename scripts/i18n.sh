#!/bin/sh -e

locales="fr_FR"

if test ! -f "Frontieres.cpp"; then
    echo "Run this script at the top of the source tree."
    exit 1
fi

for locale in $locales; do
    lupdate *.h *.cpp \
            -tr-function-alias translate+=_Q,translate+=_S \
            -ts translations/Frontieres_"$locale".ts
done
