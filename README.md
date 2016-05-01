
**Jamtaba 2** is a software to play online music jam sessions. At moment you can use Jamtaba to play with musicians around the world using [ninjam ](http://www.cockos.com/ninjam/) servers.

![Jamtaba](http://jamtaba-music-web-site.appspot.com/img/Jamtaba_2_0_14_jamming.png)

Jamtaba is a **standalone** software and a **VST plugin** (windows only at moment). Using the standalone version you can use your audio and midi devices and use VST plugins inside Jamtaba (Jamtaba standalone is a VST host). Using the VST plugin you can use Jamtaba in your preferred DAW and send your audio tracks to Jamtaba, and Jamtaba will stream these audio tracks to ninjam servers. Off course Jamtaba will receive and play the other musicians audio streams, so you can play with others and have some fun playing online!

At moment Jamtaba standalone works for Windows (32 and 64 bits) and Mac OSX (10.7+) and Linux. The VST plugin is available only for windows (32 and 64 bits).
More information and User's manual are here : https://github.com/elieserdejesus/JamTaba/wiki      
***

## Tools and dependencies
JamTaba is builded using **Qt framework**. We are using Qt for GUI, threads, sockets, json, http requests, file handling, etc. 

Jamtaba is using some libraries: [portaudio](http://www.portaudio.com/), [rtmidi](https://www.music.mcgill.ca/~gary/rtmidi/), [minimp3](http://keyj.emphy.de/minimp3/), [libvorbis and libogg](http://www.vorbis.com/). We are distributing **pre compiled static libs** just for convenience (see more details in the next sections), but feel free to compile the libs yourself!

####IMPORTANT FOR WINDOWS if you will use the pre compiled libs (recommended):
The library **portaudio** was **pre compiled** in windows using **only the ASIO flags**. So, if you don't have any **ASIO driver** installed JamTaba will emit an error message in initialization and fallback to an NullAudioDriver, and no sound will be produced. If you have no ASIO devices please install [Asio4ALL](http://www.asio4all.com/).

##Downloading Pre Compiled Libs
- To avoid store big binay files in the GitHub repository the [pre compiled libs](https://dl.dropboxusercontent.com/u/23791949/jamtaba/jamtaba2/JamTaba-static-libs.zip) are stored in my personal dropbox account. Please download this file!
- After download the **zip** file with the **pre compiled static libraries** please unpack this file inside a folder **libs** (create this folder). The folders layout will be:
```
[Jamtaba source code folder in your hard disk]
	|_ installers
	|_ libs	  
		|_ includes
		|_ static
	|_ PROJECTS
	|_ src
    |_ tests
```
***

## Compiling JamTaba Standalone

- Download and install [Qt](http://www.qt.io/download-open-source/). The Qt installer will install the **Qt Framework** and the **QtCreator**, the Qt IDE.
- Download [VST SDK](http://www.steinberg.net/en/company/developers.html)
- Unpack the VST SDK content inside a **VST_SDK** folder (you need create this folder) in Jamtaba source code path. After this you have something like that:
```
[Jamtaba source code folder in your hard disk]
	|_ PROJECTS
	|_ installers
	|_ src
    |_ tests
	|_ VST_SDK       <- create this folder
		|_ base
		|_ bin
		|_ doc
		|_ pluginterfaces
		|_ plublic.sdk
		|_ vstgui.sf
		|_ vstgui4
		|_ index.html
```
- The **QtCreator projects** are in the folder **PROJECTS**. Try open the **Standalone.pro** in QtCreator to compile **JamTaba Standalone**. 

- Compile the project in QtCreator (the green button in left side)!

***

##Compiling JamTaba Vst Plugin
- To compile JamTaba **Vst Plugin** you need run all steps described in the section **Compiling Jamtaba Standalone**, except the 2 last steps.
- In the last steps you need open the file **VstPlugin.pro** in **QtCreator**. If you try to compile this project you will got some errors, because the Vst Plugin can't be compiled using **official Qt releases** downloaded in Qt official site. These Qt releases are compiled/using shared libraries. You need a **static compiled Qt** to compile the Vst Plugin, and you need compile this special Qt version for your self. Check the [Qt docs](http://doc.qt.io/qt-5/build-sources.html) to see how compile Qt in a "static flavor". 
- Inside the folder **PROJECTS/VstPlugin** you have a file **Qt static compile flags.txt** with some flags to configure Qt before compile and reduce the size of Qt static libs. These flags will speed up the compilation process too. In my machine (a common laptop with 4 cores) the compilation process lasted 15 minutes using these flags. 
