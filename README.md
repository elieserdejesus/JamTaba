**Jamtaba 2** is a software to play online music jam sessions. At moment you can use Jamtaba to play with musicians around the world using [ninjam ](http://www.cockos.com/ninjam/) servers.

Jamtaba is a standalone software and a VST plugin. Using the standalone version you can use your audio and midi devices and use VST plugins inside Jamtaba (Jamtaba standalone is a VST host). Using the VST plugin you can use Jamtaba in your preferred DAW and send your audio tracks to Jamtaba, and Jamtaba will stream these audio tracks to ninjam servers. Off course Jamtaba will receive and play the other musicians audio streams, so you can play with others and have some fun playing online!

###Get Jamtaba source code###

1. ***Using Git***: git clone https://github.com/elieserdejesus/JamTaba.git

or 
2. ***Download the repository***: https://github.com/elieserdejesus/JamTaba/archive/master.zip

***

###Windows Development###

##Steps to build Using MinGW##

1 - Download Qt OffLine Installer for Windows MinGW: http://www.qt.io/download-open-source/
 
2 - Download VST SDK: http://www.steinberg.net/en/company/developers.html

3 - Unpack the download VST SDK content inside a "VST_SDK" folder (you need create this folder) in Jamtaba source code path. After this you have something like that:
```	
Jamtaba source code dir in your hard disk
	|_ Client
	|_ Icons
	|_ Server
	|_ VST_SDK 
		|_ base
		|_ bin
		|_ doc
		|_ pluginterfaces
		|_ plublic.sdk
		|_ vstgui.sf
		|_ vstgui4
		|_ index.html
```
4 - Run Qt installer and check the options below: 
	Qt -> Qt 5.5 -> MinGW 4.9.2 32 bit
	Qt -> Tools -> MinGW 4.9.2
	This will install Qt compiled with MinGW and install MinGW tools (compiler, debugger, etc) in [Qt install folder]/Tools. 

5 - Open QtCreator (the Qt IDE). QtCreator is located in[Qt install folder]/Tools/QtCreator.exe. If you select Tools->MinGW in setp 3 QtCreator is configured to use MinGW, this is the easy way!

6 - Open the Jamtaba.pro file in QtCreator (use the button "Open Project in QtCreator start screen).

7 - [Optional Step]: In QtCreator click in "Projects" in left side to open the "Build Settings" page. In "Build Steps" section expand you will see "qmake" and "Make" details. Open "Make" details (click in the combo in right side). Now you see a text field for "Make arguments". Put "-j 4" in this text field, where "4" is the number of cores in your processor. This will speed up the compilation using all your processor cores.

8 - Have fun! This is the most important step!



##Build Using msvc##
TODO
