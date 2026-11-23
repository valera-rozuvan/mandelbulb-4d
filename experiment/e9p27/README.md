# combine 3D fractals using physics

to compile on a Linux system (tested on a Ubuntu based distro)

```shell
g++ -O3 -march=native -ffast-math -fopenmp spacetime_hybrid_pbr.cpp -o shpbr
```

then you can run `./shpbr`, and optionally pass some params to it, to get an image file

## sample renders

this folder contains high quality 1600x900 renders I landed on while exploring

the file `params.txt` contains exact invocation that was used for each render

keep in mind that I produced these on an Intel based machine with 8 cores, under Linux Mint

```text
g++ version 13.3.0
Linux Mint 22.1
```
