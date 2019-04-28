FROM ubuntu:16.04 

ENV TERM=xterm-256color
ENV OF_VERSION "0.10.1-patched"
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
        gstreamer1.0-plugins-ugly \
        ccache \
        vim \
        unzip \
        rsync \
        git \
        ca-certificates

# Install openFrameworks for linux64 and linuxarmv6l
RUN git clone https://github.com/lucasrangit/openFrameworks.git --branch ${OF_VERSION} --single-branch /opt/openFrameworks && \
    /opt/openFrameworks/scripts/linux/download_libs.sh && \
    /opt/openFrameworks/scripts/linux/ubuntu/install_dependencies.sh -y && \
    /opt/openFrameworks/scripts/linux/compileOF.sh -j`nproc` && \
    /opt/openFrameworks/scripts/linux/download_libs.sh --platform linuxarmv6l && \
    /opt/openFrameworks/scripts/ci/linuxarmv6l/install.sh && \
    TARGET=linuxarmv6l /opt/openFrameworks/scripts/ci/linuxarmv6l/build.sh

# Add user that matches host user
ARG BUILD_UID=1000
ARG BUILD_GID=1000
RUN (test $(getent group $BUILD_GID) || addgroup -gid $BUILD_GID docker) && \
    (test $(getent passwd $BUILD_UID) || adduser --disabled-password --gecos "" -uid $BUILD_UID -gid $BUILD_GID docker) && \
    adduser docker sudo && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers.d/docker

# Build needs write access to /opt/openFrameworks/addons/obj for some reason
RUN chown docker. /opt/openFrameworks/addons/obj

USER $BUILD_UID
WORKDIR /home/docker/hackandtell-raspberry
