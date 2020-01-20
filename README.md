**Frontières** est un échantillonneur granulaire.

*Ceci est une reprise non-officielle de la version 0.4 libre du logiciel [Borderlands](http://borderlands-granular.com/).  Voir [sa page sur linuxmao.org](http://linuxmao.org/borderlands). Vous trouverez des tas de notes concernant le logiciel originel en version 0.4 dans les fichiers [README.txt](README.txt) et [README-2.txt](README-2.txt).*


## Construction

[![Build Status](https://travis-ci.com/linuxmao-org/Frontieres.svg?branch=master)](https://travis-ci.com/linuxmao-org/Frontieres)

## Téléchargement

**Version en développement**

- [:package::penguin: Paquet GNU/Linux 64 bits : pour Debian, Ubuntu, LibraZiK, Mint](https://github.com/linuxmao-org/Frontieres/releases/download/automatic/frontieres-linux64.deb)

## Généralités

Le système de construction est cmake, et le cadriciel visuel est Qt5.


## Debian Buster
 
Vous aurez besoin des paquets suivants : `cmake build-essential libasound2-dev libglu1-mesa-dev libjack-jackd2-dev` ou `libjack-dev libsndfile1-dev libsoxr-dev liblo-dev mesa-common-dev libpulse-dev` et `pkg-config`.

Ainsi que les paquets Qt5 suivants : `libqt5opengl5-dev, libqt5x11extras5-dev, qtbase5-dev, qttools5-dev,` et `qttools5-dev-tools`.

## Debian Stretch
 
Vous aurez besoin des paquets suivants : `cmake build-essential libasound2-dev libglu1-mesa-dev libjack-jackd2-dev` ou `libjack-dev libsndfile1-dev libsoxr-dev liblo-dev mesa-common-dev libpulse-dev` et `pkg-config`.

Ainsi que les paquets Qt5 suivants : `libqt5opengl5-dev, libqt5x11extras5-dev, qtbase5-dev, qttools5-dev,` et `qttools5-dev-tools`.

_(note : certains sont peut être non-nécessaires)_
