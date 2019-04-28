#!/usr/bin/env bash
set -o errexit
set -o xtrace

# Ensure build is performed inside a container
if ! grep -c docker /proc/1/cgroup > /dev/null; then
    image_tag="$(basename $PWD | sed 's/_/-/g' | cut -c 1-40 | tr '[:upper:]' '[:lower:]'):$(git rev-parse --short HEAD)"
    docker build --build-arg BUILD_UID=$(id -u) --build-arg BUILD_GID=$(id -g) --tag local/$image_tag .
    if [[ ${1} == "--shell" ]]; then
        docker run --name $(basename $PWD) -it --rm -v $PWD:/home/docker/$(basename $PWD) local/$image_tag bash
    else
        docker run --name $(basename $PWD) --rm -v $PWD:/home/docker/$(basename $PWD) local/$image_tag "$0" "$*"
    fi
    exit $?
fi

rm bin/hackandtell-raspberry

if [[ ${1} == "armv6" ]]; then
    source cross.env
fi

make -j`nproc`