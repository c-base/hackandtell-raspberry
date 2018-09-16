FROM ubuntu:16.04 

ENV TERM=xterm-256color
ENV OF_VERSION "0.9.8"
ENV OF_ROOT "/opt/openFrameworks"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update; \
    apt-get install --no-install-recommends -y \
        sudo \
        build-essential \
        wget \
        apt-utils \
	lsb-release \
        libmpg123-dev \
        gstreamer1.0 \
        gstreamer1.0-plugins-ugly

# Install OpenFrameworks
# based off https://openframeworks.cc/setup/raspberrypi/raspberry-pi-getting-started/
RUN wget --no-check-certificate http://openframeworks.cc/versions/v${OF_VERSION}/of_v${OF_VERSION}_linux64_release.tar.gz && \
    mkdir -p /opt/openFrameworks && \
    tar -xzvf of_v${OF_VERSION}_linux64_release.tar.gz -C /opt/openFrameworks --strip-components 1 && \
    /opt/openFrameworks/scripts/linux/ubuntu/install_dependencies.sh -y && \
    /opt/openFrameworks/scripts/linux/compileOF.sh -j3

# Add user that matches host user
ARG BUILD_UID=1000
ARG BUILD_GID=1000
RUN (test $(getent group $BUILD_GID) || addgroup -gid $BUILD_GID docker) && \
    (test $(getent passwd $BUILD_UID) || adduser --disabled-password --gecos "" -uid $BUILD_UID -gid $BUILD_GID docker) && \
    adduser docker sudo && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers.d/docker

# Build needs write access to /opt/openFrameworks/addons/obj for some reason
RUN mkdir /opt/openFrameworks/addons/obj && \
    chown docker. /opt/openFrameworks/addons/obj

USER $BUILD_UID
WORKDIR /home/docker/hackandtell-raspberry

