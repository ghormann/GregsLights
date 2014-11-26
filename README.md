
This is the software that runs our lighting display (http://thehormanns.net).   The code id designed to run on Linux and includes the build files for CodeBlocks 12.11



Genereate a make file using: 
  * ../../cbp2make/cbp2make-stl-rev147/bin/Release/cbp2make -in GregsLights.cbp 

Build from Make file using
  * make -f GregsLights.cbp.mak release


### Contributions ###

* Huge thanks to the xLights team (https://github.com/smeighan/xLights).  Much of the Serial Level control code is modeled after their design.
