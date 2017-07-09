# mandelbulb-4d

State-of-the-art rendered, fine tuned to produce highly realistic visuals of arbitrary complex fractal 3D worlds


## Build instructions

Currently this is a work in progress. On windows, I build binaries using cmake and MinGW-64-bit (from RubenVB for
[64-bit Windows target](http://sourceforge.net/projects/mingw-w64/files/Toolchains)). On Linux, I build using cmake and
the GCC collection. On Mac OS, I build using cmake and the command line developer tools. Most libraries are included as
part of the source. You only need to make sure you have OpenCL development files (headers) on your system.

Sample steps to build on Linux:

```
  git clone __ADDRESS_OF_THIS_REPO__
  cd mandelbulb-4d
  mkdir build
  cd build

  cmake ../src
  make -j 4

  cp ../src/hello.cl .

  ./simple_window
```

Cheers!


## License

This project is licensed under the MIT license. See [LICENSE](LICENSE) for more details.
