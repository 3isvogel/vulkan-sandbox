# vulkan-sandbox

> Learning Computer Graphics programming principles, Vulkan API and Shaders

This repo will follow the resources listed on [the Easy Blog](https://3isvogel.github.io/blogs/#3d-computer-graphic), using **C++** as the main programming language

## Build
To build the project simply run:
```sh
make
```

To speed up the build remember to set the number of jobs
> Linux
```sh
export MAKEFLAGS="-j $(nproc --all)"
```
> MacOS
```sh
export MAKEFLAGS="-j $(sysctl -n hw.ncpu)"
```
> Windows
```
¯\_(ツ)_/¯
```