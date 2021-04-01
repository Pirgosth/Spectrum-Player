# Spectrum Player - Watch your favorites musics 
## Introduction

Spectrum Player is a simple music player with real time FFT spectrum that allow you to watch your favorites musics!

## Installation

First, make sure to have both `sfml-devel` and `mpg123-devel` installed on your system before compiling.

On Fedora, you can simply run `sudo dnf install sfml-devel mpg123-devel mpg123`

Next you just have to run `make` at the root of this repository to build this project for your system.

## Usage

You can run it in console without any args, it will ask you a `file path` pointing on the file you actually want to play (currently, it only supports WAV and MP3)

You can also run it directly without console interface by specifying `file path` as first argument

Exemple: `./spectrum Toto-By-Africa.mp3`