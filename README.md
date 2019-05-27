# RaspberryPi version of the great Hack&amp;Tell countdown timer.

Simple countdown timer with fullscreen graphical display on the composite video out and extension
to the Matelight 0.00061 Megapixel display: http://github.com/jaseg/matelight

Needs http://openframeworks.cc/ to compile.

## How to checkout 

    cd /path/to/of_v0.8.4_osx_release/apps/myApps/
    git clone https://github.com/c-base/hackandtell-raspberry.git
    cd hackandtell-raspberry
    make && make run 
    # or use XCode or other IDE to compile

## Build

* Pass the `--shell` argument to get an interactive shell inside the build container.
* Pass the `armv6` argument to build for the armv6 (e.g. Raspberry Pi).

* `./build.sh`

### Runtime Dependencies

* Ubuntu 16.04: `sudo apt install libfreeimage3 libglfw3 liburiparser1`

### Run

* `bin/hackandtell-raspberry`
