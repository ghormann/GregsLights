
This is the software that runs our lighting display (http://thehormanns.net).   The code id designed to run on Linux and includes the build files for CodeBlocks 13.xx

Required LIbaries (install using apt-get or simular)

# Ubuntu 22.04
sudo apt-get install build-essential libpng++-dev libgtk2.0-dev libgstreamer-plugins-base1.0-dev freeglut3-dev libmpg123-dev libwxgtk-media3.0-gtk3-dev libncurses5-dev libjsoncpp25  libjsoncpp-dev libmosquittopp1 libmosquittopp-dev libgraphicsmagick++1-dev graphicsmagick-libmagick-dev-compat

# Ubuntu 18
sudo apt-get install build-essential libpng++-dev libgtk2.0-dev libgstreamer-plugins-base1.0-dev freeglut3-dev libmpg123-dev libwxgtk-media3.0-dev libncurses5-dev libjsoncpp1 libjsoncpp-dev libmosquittopp1 libmosquittopp-dev libgraphicsmagick++1-dev graphicsmagick-libmagick-dev-compat


# Other Info
JSON Parser: https://github.com/open-source-parsers/jsoncpp/wiki

Basic source for setting up SSL: http://www.steves-internet-guide.com/mosquitto-tls/


Genereate a make file using: 
  * ../../cbp2make -in GregsLights.cbp 

Build from Make file using
  * make -f GregsLights.cbp.mak release


### Contributions ###

* Huge thanks to the xLights team (https://github.com/smeighan/xLights).  Much of the Serial port and E131 Level code is modeled after their design.
