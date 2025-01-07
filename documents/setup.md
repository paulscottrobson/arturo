mkdir /aux/pico

cd /aux/pico

git clone https://github.com/raspberrypi/pico-sdk

cd /aux/pico/pico-sdk

git submodule update --init



git clone https://github.com/Wren6991/PicoDVI

cd /aux/pico/picoDVI



cd .... to the project directory

git clone https://github.com/paulscottrobson/arturo



Go to build_env directory and edit config.make to support the directories you used

export PICO_SDK_PATH := /aux/pico/pico-sdk/
export PICO_DVI_PATH := /aux/pico/PicoDVI/

Go to "image" directory and build with Make

cd image

make build

It may crash with not finding mbedtls/sha256.h and or hardware/sha256.h

cp /aux/pico/pico-sdk/lib/mbedtls/include/mbedtls/ include/ -Rv

 cp /aux/pico/pico-sdk/src/rp2_common/hardware_sha256/include/hardware/ include/ -Rv