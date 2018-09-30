#!/bin/sh -e
set -x

locales="fr"

po_update() {
    if test -f "$1"; then
        msgmerge -U "$1" "$1"t
    else
        msginit -i "$1"t -l "$locale" -o "$1"
    fi
}

for locale in $locales; do
    mkdir -p po/"$locale"
    xgettext -k_ -L C++ -c -s -o po/"$locale"/Borderlands.pot *.{h,cpp}
    po_update po/"$locale"/Borderlands.po
    rm -f po/"$locale"/*.pot
done
