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

## Host Build with Docker

* `./build.sh`

### Runtime Dependencies

* Ubuntu: `sudo apt install libfreeimage3`

### Run

* `bin/hackandtell-raspberry`

