FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    libpng++-dev \
    libgtk2.0-dev \
    libgstreamer-plugins-base1.0-dev \
    freeglut3-dev \
    libmpg123-dev \
    libwxgtk-media3.0-gtk3-dev \
    libncurses5-dev \
    libjsoncpp25 \
    libjsoncpp-dev \
    libmosquittopp1 \
    libmosquittopp-dev \
    libgraphicsmagick++1-dev \
    graphicsmagick-libmagick-dev-compat \
    cbp2make \
    curl \
    git \
    && rm -rf /var/lib/apt/lists/*

# Install Node.js 22 via NodeSource
RUN curl -fsSL https://deb.nodesource.com/setup_22.x | bash - \
    && apt-get install -y nodejs \
    && rm -rf /var/lib/apt/lists/*

# Install Claude CLI globally
RUN npm install -g @anthropic-ai/claude-code

# Create vscode user (matches standard devcontainer convention)
RUN groupadd --gid 1000 vscode \
    && useradd --uid 1000 --gid 1000 -m -s /bin/bash vscode \
    && apt-get update && apt-get install -y sudo \
    && echo "vscode ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/vscode \
    && rm -rf /var/lib/apt/lists/*

USER vscode
