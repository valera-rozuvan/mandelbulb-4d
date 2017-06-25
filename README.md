# mandelbulb-4d
State-of-the-art rendered, fine tuned to produce highly realistic visuals of arbitrary complex fractal 3D worlds

## Build instructions

Currently this is a work in progress. On windows, I build binaries using cmake and
MinGW-64-bit (from RubenVB for
[64-bit Windows target](http://sourceforge.net/projects/mingw-w64/files/Toolchains)).
On Linux, I build using cmake and the GCC collection. Most libraries are included
as part of the source. You only need to make sure you have OpenCL headers and
development files on your system.

On Linux:

```
  git clone ADDRESS_OF_THIS_REPO
  cd mandelbulb-4d
  mkdir build
  cd build

  cmake ../src2
  make -j 4

  ./simple_window
```

Cheers!
