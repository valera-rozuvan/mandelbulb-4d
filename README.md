# mandelbulb-4d

```text
!! Work in progress !!

a lot remains to be done
```

artistic tool for 3D fractal generation, including the Mandelbulb

## Introduction

![Mandelbulb 4D v0.0.5](img/screenshot_v0.0.5.png)

This project was born from the discussion [Remake of MB3D](http://www.fractalforums.com/mandelbulb-3d/mb3d-v2/) over at [Fractal Forums](http://www.fractalforums.com/). For the theory, please read [Mandelbulb (From Wikipedia, the free encyclopedia)](https://en.wikipedia.org/wiki/Mandelbulb).

## Similar projects

- [Mandelbulber v2](https://github.com/buddhi1980/mandelbulber2)
- [Mandelbulb3D](https://github.com/thargor6/mb3d)
- [Fragmentarium](https://github.com/Syntopia/Fragmentarium)

## Build instructions

Currently this is a work in progress. On windows, I build binaries using cmake and MinGW-64-bit (from RubenVB for
[64-bit Windows target](http://sourceforge.net/projects/mingw-w64/files/Toolchains)). On Linux, I build using cmake and
the GCC collection. On Mac OS, I build using cmake and the command line developer tools. Most libraries are included as
part of the source. You need to make sure you have OpenCL development files (headers) on your system.
Also, on a Debian-based Linux distribution, you should install the following dependencies:

```
sudo apt-get install build-essential gcc g++ make automake autoconf cmake\
  libgl1-mesa-dev ocl-icd-opencl-dev libglfw3-dev libxinerama-dev libxcursor-dev\
  freeglut3-dev libboost-random-dev
```

Sample steps to build on Linux:

```
  git clone __ADDRESS_OF_THIS_REPO__
  cd mandelbulb-4d
  mkdir build
  cd build

  cmake ../src
  make -j 4

  cp ../src/hello.cl .

  ./mb4d_gui
```

Cheers!

---

## license

The project `'mandelbulb-4d'` is licensed under the MIT License.

See [LICENSE](./LICENSE) for more details.

The latest source code can be retrieved from one of several mirrors:

1. [github.com/valera-rozuvan/mandelbulb-4d](https://github.com/valera-rozuvan/mandelbulb-4d)

2. [gitlab.com/valera-rozuvan/mandelbulb-4d](https://gitlab.com/valera-rozuvan/mandelbulb-4d)

3. [git.rozuvan.net/mandelbulb-4d](https://git.rozuvan.net/mandelbulb-4d)

Copyright (c) 2016-2022 [Valera Rozuvan](https://valera.rozuvan.net/)
