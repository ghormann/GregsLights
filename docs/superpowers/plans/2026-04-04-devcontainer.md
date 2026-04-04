# DevContainer Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create a `.devcontainer` so the GregsLights project opens in a fully-configured Ubuntu 22.04 environment with all C++ build dependencies, X11 display support, Node.js 22, and the Claude CLI.

**Architecture:** A single `Dockerfile` (from `ubuntu:22.04`) installs all apt packages, Node.js 22, and Claude CLI. A `devcontainer.json` references that Dockerfile, forwards X11 display variables from the WSL2 host, and declares VS Code extensions.

**Tech Stack:** Docker, Ubuntu 22.04, GCC/G++, cbp2make, wxWidgets, GraphicsMagick, mosquittopp, jsoncpp, Node.js 22, Claude CLI (`@anthropic-ai/claude-code`), VS Code devcontainers.

---

## File Map

| Action | Path | Responsibility |
|--------|------|----------------|
| Create | `.devcontainer/Dockerfile` | Ubuntu 22.04 image with all build deps, Node 22, Claude CLI |
| Create | `.devcontainer/devcontainer.json` | VS Code devcontainer config, extensions, X11 forwarding |

---

### Task 1: Create the Dockerfile

**Files:**
- Create: `.devcontainer/Dockerfile`

- [ ] **Step 1: Create `.devcontainer/Dockerfile`**

```dockerfile
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
    codeblocks \
    curl \
    git \
    && rm -rf /var/lib/apt/lists/*

# Install Node.js 22 via NodeSource
RUN curl -fsSL https://deb.nodesource.com/setup_22.x | bash - \
    && apt-get install -y nodejs \
    && rm -rf /var/lib/apt/lists/*

# Install Claude CLI globally
RUN npm install -g @anthropic-ai/claude-code
```

- [ ] **Step 2: Verify the Dockerfile builds**

Run from the repo root:
```bash
docker build -f .devcontainer/Dockerfile .devcontainer/
```

Expected: Build completes with no errors. Final lines resemble:
```
Successfully built <image-id>
```

If NodeSource fails (network issue), check that `curl` is available and the NodeSource URL is reachable.

- [ ] **Step 3: Spot-check key tools inside the image**

```bash
docker run --rm $(docker build -q -f .devcontainer/Dockerfile .devcontainer/) \
  bash -c "g++ --version && cbp2make --version && node --version && claude --version"
```

Expected output (versions may vary):
```
g++ (Ubuntu ...) 11.x.x
cbp2make rev.147
v22.x.x
claude ...
```

- [ ] **Step 4: Commit**

```bash
git add .devcontainer/Dockerfile
git commit -m "feat: add devcontainer Dockerfile based on Ubuntu 22.04"
```

---

### Task 2: Create devcontainer.json

**Files:**
- Create: `.devcontainer/devcontainer.json`

- [ ] **Step 1: Create `.devcontainer/devcontainer.json`**

```json
{
  "name": "GregsLights",
  "build": {
    "dockerfile": "Dockerfile"
  },
  "workspaceFolder": "/workspaces/GregsLights",
  "remoteEnv": {
    "DISPLAY": "${localEnv:DISPLAY}",
    "WAYLAND_DISPLAY": "${localEnv:WAYLAND_DISPLAY}"
  },
  "mounts": [
    "source=/tmp/.X11-unix,target=/tmp/.X11-unix,type=bind,consistency=cached"
  ],
  "customizations": {
    "vscode": {
      "extensions": [
        "ms-vscode.cpptools",
        "ms-vscode.cpptools-extension-pack",
        "ms-vscode.makefile-tools"
      ]
    }
  }
}
```

- [ ] **Step 2: Validate JSON is well-formed**

```bash
python3 -c "import json; json.load(open('.devcontainer/devcontainer.json')); print('JSON valid')"
```

Expected:
```
JSON valid
```

- [ ] **Step 3: Commit**

```bash
git add .devcontainer/devcontainer.json
git commit -m "feat: add devcontainer.json with VS Code extensions and X11 forwarding"
```

---

### Task 3: End-to-end verification (manual)

This task is performed by opening the repo in VS Code and using the devcontainer.

- [ ] **Step 1: Open in devcontainer**

In VS Code: `Ctrl+Shift+P` → `Dev Containers: Reopen in Container`

Expected: Container builds (or pulls from cache) and VS Code reconnects inside it.

- [ ] **Step 2: Verify C++ extensions loaded**

In VS Code, open any `.cpp` file (e.g. `GregsLights/main.cpp`). Expected: IntelliSense underlines resolve, no "extension not found" errors in the Extensions panel.

- [ ] **Step 3: Verify build tools work**

Open the VS Code terminal (inside the container) and run:

```bash
cd GregsLights
make -f GregsLights.cbp.mak release
```

Expected: Compiles successfully, `bin/Release/GregsLights` is produced.

- [ ] **Step 4: Verify cbp2make works**

```bash
cbp2make -in GregsLights.cbp
```

Expected: Regenerates `GregsLights.cbp.mak` with no errors.

- [ ] **Step 5: Verify Claude CLI works**

```bash
claude --version
```

Expected: Prints Claude CLI version string.

- [ ] **Step 6: Verify X11 forwarding (Graphics target)**

```bash
make -f GregsLights.cbp.mak graphics
./GregsLights
```

Expected: The GregsLights GUI window opens on your Windows desktop via WSLg. If `DISPLAY` is not set, run `echo $DISPLAY` and verify it shows `:0` or similar.
