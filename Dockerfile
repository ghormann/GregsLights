FROM ubuntu:24.04

# Set environment to non-interactive to avoid prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies from the README
RUN apt-get update && apt-get install -y \
    build-essential \
    libpng++-dev \
    libgtk2.0-dev \
    libgstreamer-plugins-base1.0-dev \
    freeglut3-dev \
    libmpg123-dev \
    libwxgtk-media3.2-dev \
    libncurses-dev \
    libjsoncpp25 \
    libjsoncpp-dev \
    libmosquittopp1 \
    libmosquittopp-dev \
    libgraphicsmagick++1-dev \
    graphicsmagick-libmagick-dev-compat \
    cbp2make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the source code
COPY . .

# Build the application
RUN cd GregsLights && \
    cbp2make -in GregsLights.cbp && \
    make -f GregsLights.cbp.mak release

# Ubuntu 24.04 ships with a built-in 'ubuntu' user (UID 1000); use it directly
USER ubuntu

# Default environment variables for additional options (empty by default)
ENV ADDITIONAL_OPTIONS=""

# Entry point that always forces -l (log mode) and includes other options from env vars
ENTRYPOINT ["/bin/sh", "-c", "/app/GregsLights/bin/Release/GregsLights -l ${ADDITIONAL_OPTIONS}"]
