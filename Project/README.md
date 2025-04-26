# Building
These instructions are for the Linux build but most apply to the Windows build as well (at least from inside
QTCreator).

## QTCreator
Most dev is done in QTCreator.

You need to install QTCreator and configure a kit for building with QT5 using gcc.

Then open the .pro file in this directory.  Configure QTCreator for a Desktop 5 build.

Press the green button to build and run.

## Command line
You can also build from the command line.

Process for command line build (in Linux anyway):
 - cd into WhippyTerm dir
    - **cd WhippyTerm**
 - make a build directory
    - **mkdir build**
 - go into new build directory
    - **cd build**
 - run qmake (you must have the QT build env installed and setup)
    - **qmake ../Project/WT.pro -spec linux-g++**
 - this makes a Makefile in the build directory. run the make file (maybe with -j$(nproc) for quicker builds)
    - **make**
 - you will find the "WhippyTerm" exe in the build directory.

### Installing QT
These are some hints for how to install the needed dev env.

#### Ubuntu 24.04
Run this line:
 - **sudo apt install build-essential qt5-qmake qtbase5-dev qtmultimedia5-dev**
