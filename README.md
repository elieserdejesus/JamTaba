**Jamtaba 2** is a software to play online music jam sessions. At moment you can use Jamtaba to play with musicians around the world using [ninjam ](http://www.cockos.com/ninjam/) servers.

Jamtaba is a standalone software and a VST plugin. Using the standalone version you can use your audio and midi devices and use VST plugins inside Jamtaba (Jamtaba standalone is a VST host). Using the VST plugin you can use Jamtaba in your preferred DAW and send your audio tracks to Jamtaba, and Jamtaba will stream these audio tracks to ninjam servers. Off course Jamtaba will receive and play the other musicians audio streams, so you can play with others and have some fun playing online!

At moment Jamtaba standalone works for Windows (32 and 64 bits) and Mac OSX (10.7+). The VST plugin is available only for windows (32 and 64 bits).
More information and User's manual are here : https://github.com/elieserdejesus/JamTaba/wiki
***

###Tools and dependencies###
JamTaba is builded using Qt. We are using Qt for many things: Gui, threads, sockets, json, http requests, file handling, etc. At moment Jamtaba is very coupled with Qt. This is a good thing if you like Qt, but can be terrible if you don't like. 

Jamtaba is using some libraries: portaudio, rtmidi, minimp3, libvorbis and libogg. We are distributing pre compiled static libs for some platforms (Windows 32 and 64 bits and Mac OSX 64 bits). The linux port is in the TODO list, but we don't have nothing interesting enough yet.

IMPORTANT FOR WINDOWS :
The library portaudio was compiled in windows using only the ASIO flags. So, if you don't have any ASIO driver installed JamTaba will emit an error message in initialization and fallback to an NullAudioDriver, and no sound will be produced.

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

***

##Build Using msvc##
TODO

***

##Working with Git -ToolTips-##
I use git in command line prompt.  
My Git knowledge is limited, but it is enough to let us rock !

Let' me share the 2 things I know about Git, maybe this can usefull for you too:

**When I will work in something new (bug or issue):**
```
git checkout -b "branchName"  //this create a new branch with  the specified name
```
**When I've finished the work with the branch "branchName":**
```
git add .  				  //to add all changes (don't forget the dot in the command!)
git commit -m "my cool commit message"    //to commit/confirm all changes
```

**Now I go back to the master and merge master with the branch:**
```
git checkout master    //backing to master branch
git merge branchName   //master is merged with the "branchName" branch

git push               //to send the files for the remote repository
```
You are done !
