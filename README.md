# HorusBinary_Tracker
4FSK HorusBinary V1 and V2 compatible HAB tracker for SX1278 and Arduino Pro mini

# Description
HorusBinary_Tracker is a sketch for tracking High Altitude Balloons using a SX127x radio chip and an Arduino Pro Mini or compatible.
It will transmit telemetry-data packages in HorusBinary 4FSK v1 and v2 format. 
Note that this tracker is still very much experimental.

# Decoding
The packages can be decoded and uploaded using a SSB receiver and software such as Horusbinary GUI and uploaded to the HabHub tracker
* Horusbinary Gui: https://github.com/projecthorus/horus-gui
* Habhub tracking page: https://tracker.habhub.org/
* horusdemodlib: https://github.com/projecthorus/horusdemodlib/wiki

# Code used
The radio related basecode for this sketch was taken from the horusbinary_radiolib project.
* See: https://github.com/projecthorus/horusbinary_radiolib

# Prerequisites
To use this tracker in a balloon, you need to request your own Payload ID. 
For that refer to: https://github.com/projecthorus/horusdemodlib/blob/master/payload_id_list.txt 

You will also need to set up a Habitat Payload Document, so that the payload telemetry appears on the HabHub tracker.
Refer to: http://habitat.habhub.org/genpayload/

# Hardware needed
To run this, you need the following hardware:

* 1 x Arduino Pro Mini - 3.3v (or compatible Arduino board)
* 1 x SX1278 LoRa chip (or compatible LoRa chip. Basically it will work with all sx127x series and Hope RFM9x chips)
* 1 x ATGM336H GPS or any UBlox or compatible GPS device like a NEO6 or NEO8. I recommend the ATGM336H GPS module.

# Libraries needed
You need the following libraries to compile this sketch:
* https://github.com/jgromes/RadioLib (Radiolib)
* https://github.com/mikalhart/TinyGPSPlus (tinyGPS++)

Be sure to use the latest versions.

# settings.h
Download the code and unzip in a folder with the name horusbinary_tracker. 
Load horusbinary_tracker.ino in your Arduino IDE. 
The tracker can be programmed by changing the values in settings.h. Instructions are also in there.

# uBlox GPS modules
Note that currently setting different flightmodes in uBlox GPS modules is not supported. This could cause GPS failures above 18km when using uBlox GPS modules. I recommend using the ATGM336H GPS modules.

And as always, use at your own risk.

Have Fun!

Roel.
