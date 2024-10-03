FROM ubuntu:22.04

LABEL maintainer="stwe"
LABEL version="main"
LABEL description="This is a Docker Image for MDCII with vcpkg support."

ARG DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt install -y \
    build-essential \
    git \
    cmake \
    libglu1-mesa-dev \
    libpng-dev \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    ca-certificates \
    libpulse-dev
RUN apt clean -y

# Install vcpkg
WORKDIR /vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git . && \
    ./bootstrap-vcpkg.sh

# Add vcpkg to the PATH environment variable
ENV PATH="/vcpkg:${PATH}"

# Set the default triplet
ENV VCPKG_DEFAULT_TRIPLET=x64-linux

# Install dependencies using vcpkg
RUN ./vcpkg install spdlog protobuf imgui

# Set working directory for the project
WORKDIR /MDCII

# ############################################################################################
# Copy Anno into the project first!
# Don't forget to change the path in config.ini.
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Make sure that under no circumstances does any data end up on public servers such as GitHub.
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
COPY Anno1602HistEd/ ./Anno1602HistEd/
# ############################################################################################

COPY install/ ./install/
COPY resources/ ./resources/
COPY src/ ./src/
COPY CMakeLists.txt .
COPY config.ini .
COPY ep.sh .
RUN chmod +x ep.sh

WORKDIR /MDCII/build

# Build project with cmake and vcpkg
RUN cmake -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug .. && \
    cmake --build . --parallel 8

# Entry point for the container
ENTRYPOINT [ "/MDCII/ep.sh" ]
