# Brain project

## Overview

This is a personal hobby project with an ambitious target to model a human
brain. I could write this code, but I couldn't run it in full for
many various reason. You can read the code, the article and the tests as well.
Maybe it would help you with something. Or not.

The white paper is in the `paper` directory.

## How to build the project in container

1. Install Visual Studio Code
1. Install [Docker](https://docs.docker.com/desktop/)
1. Open the project folder. It can take up to 30 minutes to build 
the container at the first start.

## How to build the project in Windows

1. Install Microsoft Visual Studio 2026 with Desktop development with C++
1. Install the latest [CMake](https://cmake.org/download/) (version >= 4.2)
1. (Optional) Install [MikTeX](https://miktex.org/download) for the article
1. Run `build.bat`

Tested in the following environment: 

- Windows 11 25H2 26200.8246
- Visual Studio 2026 18.5.2
- Windows 11 SDK 10.0.26100.7705
- CMake 4.3.2


## How to build the project in Ubuntu

1. Install the required packages
```
sudo apt install -y g++ cmake libgtest-dev gdb
```
2. (Optional) Install TeXLive for the article
```
sudo apt install -y texlive-full  
```
3. Run `build.sh`

Tested in the following environment: 

- Ubuntu 24.04
- g++ 13.3.0
- CMake 3.28.3
- GoogleTest 1.14.0


## DotArray viewer in Visual Studio

see documentation [here](vsext/README.md)

