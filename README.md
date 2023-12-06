# vulkan-sandbox

> Learning Computer Graphics programming principles, Vulkan API and Shaders

This repo will follow the resources listed on [the Easy Blog](https://3isvogel.github.io/blogs/#3d-computer-graphic), using **C++** as the main programming language

## Building project

Be sure `make` is installed and run the command:
```sh
make
```

### To enable parallel building you can export the following variables
> Linus
```sh
export MAKEFLAGS="-j $(sysctl -n hw.ncpu)"
```
> MacOS
```sh
export MAKEFLAGS="-j $(nproc)"
```
> Windows
```
¯\_(ツ)_/¯
```