# Setting up a build environment

## Downloading the Pico SDK

mkdir /aux/pico

cd /aux/pico

git clone https://github.com/raspberrypi/pico-sdk

cd /aux/pico/pico-sdk

git submodule update --init

## Download the Pico DVI core

git clone https://github.com/Wren6991/PicoDVI

cd /aux/pico/picoDVI

## Download the Arturo core

cd .... to the project directory

git clone https://github.com/paulscottrobson/arturo

## Configuring 

Go to the build_env directory and edit config.make to support the directories you used

export PICO_SDK_PATH := /aux/pico/pico-sdk/
export PICO_DVI_PATH := /aux/pico/PicoDVI/

Go to "image" directory and build with Make

cd image

make build

