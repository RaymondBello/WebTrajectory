# WebTrajectory
Web-based trajectory simulator using OpenGL

## Commands to setup dependencies
This project has the following dependencies
- SDL2 (window management)
- OpenGL 3+ 
- Emscripten 

### Setup for Linux
- Install SDL2
  `sudo apt-get install libsdl2-dev`


#### Setup for Linux



## Commands to build desktop app
```
git clone https://github.com/RaymondBello/WebTrajectory.git
cd WebTrajectory
mkdir build && cd build
cmake ..
make -j4
```

## Commands to build emscripten app
```
git clone https://github.com/RaymondBello/WebTrajectory.git
cd WebTrajectory
mkdir build-em && cd build-em
emcmake cmake ..
make -j4
```