
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

#### IMPORTANT FOR WINDOWS if you will use the pre compiled libs (recommended):
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
- In the last steps you need open the file **VstPlugin.pro** in **QtCreator**. If you try to compile this project you will got some errors, because the Vst Plugin can't be compiled using **official Qt releases** downloaded in Qt official site. These Qt releases are compiled/using shared libraries. You need a **static compiled Qt** to compile the Vst Plugin, and you need compile this special Qt version for your self. Check the [Qt docs](http://doc.qt.io/qt-5/build-sources.html) to see how compile Qt in a "static flavor". 
- Inside the folder **PROJECTS/VstPlugin** you have a file **Qt static compile flags.txt** with some flags to configure Qt before compile and reduce the size of Qt static libs. These flags will speed up the compilation process too. In my machine (a common laptop with 4 cores) the compilation process lasted 15 minutes using these flags. 

***
Detailed instructions on how to build a **static compiled Qt** to compile the Vst Plugin:

**1.-** Download Qt Sources, but only submodules used by JamTaba. This will speed up the compilation. The sources are in http://download.qt.io/archive/qt/5.5/5.5.1/submodules/

	Download the files:

	1 - qt5-opensource-src-5.5.1.zip
	2 - qtbase-opensource-src-5.5.1.zip
	3 - qtmultimedia-opensource-src-5.5.1.zip
	4 - qttools-opensource-src-5.5.1.zip
	5 - qtwebsockets-opensource-src-5.5.1.zip

**2** - Extract the dowloaded files in your preferred Qt folder, for example C:/MyQt. All files will be extracted inside 5 subfolders.

**3** - Rename the extracted subfolders removing the suffix -opensource-src-5.5.1. The renamed folders will be qtbase, qtmultimedia, qtwebsockets and qttools. It's not necessary rename the folder qt-everywhere-opensource-src-5.5.1 (see the next step).

**4** - Move all files inside the folder qt-everywhere-opensource-src-5.5.1 the your Qt folder and delete the folder qt-everywhere-opensource-src-5.5.1. The final folders/files layout will be:

![root_dir_qt_static](https://user-images.githubusercontent.com/1012741/30293084-b94d9818-970e-11e7-8103-51c0179d16d6.png)

**5** - In your Qt folder create a file named qt5vars.cmd using a text editor (notepad). Paste these script lines in this file, but REMEMBER to change the _ROOT from "C:/MyQt" to your real "Qt folder" path:

	CALL "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
	SET _ROOT=C:/MyQt

	SET PATH=%_ROOT%\qtbase\bin;%_ROOT%\gnuwin32\bin;%PATH%

	SET QMAKESPEC=win32-msvc2013
	SET _ROOT=

**6** - Open the windows prompt (cmd) and navigate to your Qt folder. The command to navigate is cd C:/QtMyQt

**7** - Run the script file created in the step 5 to setup MSVC. In the command prompt type qt5vars.cmd and press ENTER. After some seconds you will see 2 lines in the prompt, and the prompt will be waiting for new commands. IMPORTANT: This script is setuping MSVC to compile in 64 bits.

**8** - Now, copy and paste these scripts line in your prompt and press ENTER to execute (this script will configure Qt just with the modules used in JamTaba):

	configure -debug-and-release -opensource -confirm-license -static -no-sql-psql -no-sql-mysql -no-sql-odbc -no-sql-tds -no-sql-oci -no-sql-db2 -no-sql-sqlite -no-sql-sqlite2 -no-sql-ibase -no-audio-backend -no-cups -no-nis -no-dbus -nomake examples -nomake tests

* After some seconds you will see **2 messages**:

`Qt is now configured for building. Just run nmake.
To reconfigure, run nmake confclean and configure.`

**9** - Just follow the message, type the command nmake in your prompt and press ENTER. Drink a coffee, the compilation lasted 90 minutes here.

**10** - After 90 min the job is done, check your Qt folder, inside the qtbase folder you have all Qt static libs compiled.

**11** - Now configure QT to compile with the static version.


