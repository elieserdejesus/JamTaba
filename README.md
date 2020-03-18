
**JamTaba 2** is a software to play online music jam sessions. At moment you can use JamTaba to play with musicians around the world using [ninjam ](http://www.cockos.com/ninjam/) servers.

![JamTaba](http://jamtaba-music-web-site.appspot.com/img/Jamtaba_2_1_0_jamming.png)

JamTaba can be used as a **standalone** software, **VST** *(Windows)* or **AU** *(Mac OSX)* plugin. Using the standalone version you can use your camera, audio and MIDI devices and load VST/AU plugins inside JamTaba (JamTaba standalone is a VST/AU host). Using the VST/AU plugin you can use JamTaba in your preferred DAW and send your audio/video to JamTaba, and Jamtaba will stream these contents to *ninjam servers*. Off course JamTaba will receive and play the other musicians audio/video streams, so you can play with others and have some fun playing online!

## User Manual
More information and User's manual are here : https://github.com/elieserdejesus/JamTaba/wiki      


## Compatibility
At moment JamTaba standalone works for Windows (*Windows XP is not supported since v. 2.1.0*), Mac OSX (10.7+) and Linux. The **VST plugin** is available for windows only (32 and 64 bits) and an **Audio Unit (AU) plugin** is available for Mac OSX users. 

## Tools and dependencies
JamTaba is builded using **Qt framework**. We are using Qt for GUI, threads, sockets, json, http requests, file handling, etc. 

JamTaba is using some libraries: [portaudio](http://www.portaudio.com/), [rtmidi](https://www.music.mcgill.ca/~gary/rtmidi/), [minimp3](http://keyj.emphy.de/minimp3/), [libvorbis and libogg](http://www.vorbis.com/). We are distributing **pre compiled static libs** just for convenience (see more details in the next sections), but feel free to compile the libs yourself!

### Ubuntu Linux
On Ubuntu Linux, run the following command to install a missing dependency for Qt 5 Websockets:

```
sudo apt install libqt5websockets5
```

### IMPORTANT FOR WINDOWS if you will use the pre compiled libs (recommended):
The library **portaudio** was **pre compiled** in windows using **only the ASIO flags**. So, if you don't have any **ASIO driver** installed JamTaba will emit an error message in initialization and fallback to an NullAudioDriver, and no sound will be produced. If you have no ASIO devices please install [Asio4ALL](http://www.asio4all.com/).

## Downloading Pre Compiled Libs
- To avoid store big binay files in the GitHub repository the [pre compiled libs](https://www.dropbox.com/s/qckwsmaqlditwpb/JamTaba-static-libs.zip?dl=0) are stored in a dropbox account. Please download this file!
- After download the **zip** file with the **pre compiled static libraries** please unpack this file inside a folder **libs** (create this folder). The folders layout will be:
```
[JamTaba source code folder in your hard disk]
	|_ AU_SDK
	|_ installers
	|_ libs	  
		|_ includes
		|_ static
	|_ PROJECTS
	|_ src
        |_ tests
        |_ translations	
        |_ VST_SDK		
```
***

## Compiling JamTaba Standalone

- Download and install [Qt](http://www.qt.io/download-open-source/). The Qt installer will install the **Qt Framework** and the **QtCreator**, the Qt IDE.
- Download [VST SDK](http://www.steinberg.net/en/company/developers.html)
- Unpack the VST SDK content inside a **VST_SDK** folder (you need create this folder) in JamTaba source code path. After this you have something like that:
```
[JamTaba source code folder in your hard disk]
	|_ AU_SDK
	|_ installers
	|_ libs	  
	|_ includes
	|_ static
	|_ PROJECTS
	|_ src
        |_ tests
        |_ translations	
	|_ VST_SDK       <- create this folder
                |_ my_plugins
                |_ VST2_SDK
                |_ VST3_SDK
                |_ copy_vst2_to_vst3_sdk.bat
                |_ copy_vst2_to_vst3_sdk.sh

```
- The **QtCreator projects** are in the folder **PROJECTS**. Try open the **Standalone.pro** in QtCreator to compile **JamTaba Standalone**. 

- Compile the project in QtCreator (the green button in left side)!

***

## Compiling JamTaba Vst Plugin
- To compile JamTaba **Vst Plugin** you need run all steps described in the section **Compiling JamTaba Standalone**, except the 2 last steps.
- In the last steps you need open the file **VstPlugin.pro** in **QtCreator**. If you try to compile this project you will got some errors, because the Vst Plugin can't be compiled using **official Qt releases** downloaded in Qt official site. These Qt releases are compiled/using shared libraries. You need a **static compiled Qt** to compile the Vst Plugin, and you need compile this special Qt version for your self. Check [this site](https://boffinblogger.blogspot.com/2018/12/static-compile-of-qt-5120-using-visual.html) to see how compile Qt in a "static flavor". 
- Inside the folder **PROJECTS/VstPlugin** you have a file **Qt static compile flags.txt** with some flags to configure Qt before compile and reduce the size of Qt static libs. These flags will speed up the compilation process too. In my machine (a common laptop with 4 cores) the compilation process lasted 15 minutes using these flags. 

***
