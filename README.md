
This is the software that runs our lighting display (http://thehormanns.net).   The code id designed to run on Linux and includes the build files for CodeBlocks 13.xx

Required LIbaries (install using apt-get or simular)

sudo apt-get install build-essential libgtk2.0-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev freeglut3-dev libmpg123-dev libwxgtk-media3.0-dev libncurses5-dev





Genereate a make file using: 
  * ../../cbp2make/cbp2make-stl-rev147/bin/Release/cbp2make -in GregsLights.cbp 

Build from Make file using
  * make -f GregsLights.cbp.mak release


### Contributions ###

* Huge thanks to the xLights team (https://github.com/smeighan/xLights).  Much of the Serial Level control code is modeled after their design.
