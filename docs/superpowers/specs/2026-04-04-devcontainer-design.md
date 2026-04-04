# DevContainer Design for GregsLights

**Date:** 2026-04-04

## Overview

Add a `.devcontainer` to the GregsLights repo so any developer can open the project in a fully-configured Ubuntu 22.04 environment with all build dependencies, VS Code extensions, X11 display support, Node.js 22, and the Claude CLI — with no manual setup required.

## File Structure

```
.devcontainer/
  devcontainer.json    # VS Code devcontainer config
  Dockerfile           # Ubuntu 22.04 image with all deps
```

Both files live at the repo root under `.devcontainer/`.

## Dockerfile

**Base image:** `ubuntu:22.04`

**Build-time only:**
- `DEBIAN_FRONTEND=noninteractive` set as a build `ARG` (not persisted to the running container) to prevent apt-get interactive prompts (e.g., timezone configuration) during `docker build`

**Apt packages:**
```
build-essential
libpng++-dev
libgtk2.0-dev
libgstreamer-plugins-base1.0-dev
freeglut3-dev
libmpg123-dev
libwxgtk-media3.0-gtk3-dev
libncurses5-dev
libjsoncpp25
libjsoncpp-dev
libmosquittopp1
libmosquittopp-dev
libgraphicsmagick++1-dev
graphicsmagick-libmagick-dev-compat
cbp2make
curl
git
```

Note: `codeblocks` (the IDE) is intentionally omitted — the project uses `cbp2make` + `make` for building; the full CodeBlocks GUI is unnecessary in a devcontainer.

**Node.js 22:**
- Installed via the NodeSource setup script (`setup_22.x`)
- Provides the `node` and `npm` binaries

**Claude CLI:**
- Installed globally: `npm install -g @anthropic-ai/claude-code`

## devcontainer.json

- **Build:** references local `./Dockerfile`
- **Workspace folder:** `/workspaces/GregsLights`
- **Display forwarding:** `DISPLAY` environment variable forwarded from host via `remoteEnv`. On WSLg (WSL2), this is sufficient — WSLg provides X11 via the DISPLAY proxy; no socket mount is needed or supported.
- **No postCreateCommand** — builds are run manually by the developer

**VS Code extensions:**
- `ms-vscode.cpptools-extension-pack` — C/C++ IntelliSense, debugging, and extended tooling (includes `ms-vscode.cpptools`)
- `ms-vscode.makefile-tools` — Makefile support for the generated `.mak` files

## Build Workflow (inside container)

```bash
cd GregsLights

# Regenerate makefiles from CodeBlocks project file (when .cbp changes)
cbp2make -in GregsLights.cbp

# Build individual targets
make -f GregsLights.cbp.mak debug
make -f GregsLights.cbp.mak release
make -f GregsLights.cbp.mak graphics

# Run Graphics target (X11 forwarded via WSLg on WSL2)
./GregsLights
```

## Design Decisions

- **No Mosquitto broker service** — only client libraries (`libmosquittopp1`, `libmosquittopp-dev`) are installed. The broker runs externally.
- **`DEBIAN_FRONTEND` scoped to build only** — kept as a Dockerfile `ARG` so it doesn't affect the interactive container session.
- **X11 via WSLg** — on WSL2, WSLg provides X11 through the `DISPLAY` env var proxy. The `/tmp/.X11-unix` socket is virtual and cannot be bind-mounted into Docker on WSLg, so only `DISPLAY` forwarding is used. No Xvfb or manual X server setup required.
- **`WAYLAND_DISPLAY` omitted** — without also mounting `$XDG_RUNTIME_DIR`, forwarding the variable alone is non-functional. Dropped in favour of X11-only forwarding via `DISPLAY`.
- **cbp2make included** — allows regenerating the Makefile after changes to `GregsLights.cbp`.
