# Trying to build a small engine for Vulkan

??? iteration, didn't like the (??? - 1) one, this thing will go on forever

# vulkan-sandbox

> Learning Computer Graphics programming principles, Vulkan API and Shaders

This repo will follow the resources listed on [the Easy Blog](https://3isvogel.github.io/blogs/#3d-computer-graphic), using **C++** as the main programming language

[vulkan-sandbox](#vulkan-sandbox)
- [Trying to build a small engine for Vulkan](#trying-to-build-a-small-engine-for-vulkan)
- [vulkan-sandbox](#vulkan-sandbox)
  - [Building project](#building-project)
    - [Make job flag](#make-job-flag)
  - [TODO:](#todo)
  - [Other references](#other-references)

## Building project

Be sure `make` is installed and run the command:
```sh
make
```

### Make job flag
you can export the following variables
> Linux
```sh
export MAKEFLAGS="-j $(nproc)"
```
> MacOS
```sh
export MAKEFLAGS="-j $(sysctl -n hw.ncpu)"
```
> Windows
```
¯\_(ツ)_/¯
```

## TODO:
- [ ] Will have to add debugger callbacks

## Other references

- [ ] [Enabling extensions](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap50.html)
- [ ] [Debug utils](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap51.html)
- [ ] [More advanced device selection](https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/03_Physical_devices_and_queue_families.html#_base_device_suitability_checks)