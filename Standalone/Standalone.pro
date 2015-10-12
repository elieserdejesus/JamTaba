#Drumazon deu problema no mac. Talvez também esteja dando problema no windows. O Torben atualizou o plugin
    #e foi dando problema em outros plugins da mesma empresa.

#é possível escanear os plugins em outro processo para evitar os crashs?
    #http://www.cantabilesoftware.com/blog/posts/40-Robust-Plugin-Scanning

#quando limpo o chat os botões de votação não são removidos

#gaz sugeriu ter a possibilidade de carregar diferentes setups

#ver o problema do mac, talvez a solução seja desativar o ssl. Se eu fizer
    #isso os usuários de versões anteriores vão parar.

#botão enter da sala não atualizou quando diminuiu a quantidade de usuários

#sistema de versão deu problema, não avisou que tem versão mais nova.Mas avisou quando rodei local.

#ideia do Echnaton para o chord progression
#https://dl.dropboxusercontent.com/u/7757890/jamtaba-chords.jpg
    #Echnaton deu uma ideia muito legal para um sistema de voto de chord progression,
        #ver a mensagem no ninbot

#Ideias do stringer sobre cores
    #The simplest idea I can share beforehand is that you might replace the light
    #grey backgrounds with a very light (semi-transparent or mixed with white)
    #pastel of the tan/yellow color to give everything a light, but warm feel.
    #The large darker bands and footer could be slightly less transparent,
    #but still light variation of the orange which is very compatible with
    #the yellow/tan (a washed out, semi-transparent version of the green might
    #also be useful for larger blocks like a feature box.)

    #Finally, the hard colors might be reserved just for headers, menu bars, tabs,
    #sub-menus, borders or other elements that you want to stand out
    #(like the yellow/tan color on the installer window border you just sent...
    #might be a nice border for the app) You run the risk of getting a website
    #or app that looks like a toucan if you overdo these lol.



#ele também falou alguma coisa sobre armazenar as progressões. Ver
    #a mensagem privada no ninbot

#change log
#1 - little change in chat translate button
#2 - Using dial in pan and improvements in local controls ui
#3 - fix minor issues in ui
#4 - fixed the problem in plugin chain - only the last vsti was generating sound
#5 - fixed the bugs in KorgLegacy plugins
#6 - new logo by Stringer
#7 - new colors?
#8 - showing public rooms in a grid
#8 - fixed the order in public rooms by musicians count

# ---------------------------

# pulo das salas não funciona sempre - talvez eu não esteja desconectando todos os signals

#hypersonic 2

#ver se acho o problema do maldito LegacyCell

#stringer disse que o programa não abre. Instalar o JamKazam para ver.

#sampletank 2 deu problema quando abro uma segunda janela e tento fechar
    #a thread da gui está morrendo

#-------------------------------- PRIMEIRO RELEASE ----------------------------------------
# Deu problema no MIDI no windows em 64 bits. Precilei compilar a portmidi  como MT ao invés de MD, port isso está dando
    #o warning sobre a mistura de libs MT e MD. Será que eu poderia usar tudo MT entao?
    #isso me deu várias horas de dor de cabeça, achei essa solução praticamente na sorte.

#--------------
#importante para VST plugin no MAC com Qt. ou seja é possível fazer o plugin VST:
    #https://forum.qt.io/topic/13731/solved-qt-gui-in-vst-plug-in-clicking-menu-bar-crashes-the-host

#---------------------------------------------------------
#separar o carregamento do plugin VST da instância. No momento uma instância é criada e depois é que o plugin é carregado. Pra mim
#isso é um bad design
#---------------------------------------------------------
#3 - Preciso melhorar o resampling aplicando o low pass.
    #o low pass mais simples: https://ccrma.stanford.edu/~jos/fp/Definition_Simplest_Low_Pass.html

    #gera código para low pass: http://www-users.cs.york.ac.uk/~fisher/cgi-bin/mkfscript

    #That’s it—to double the sample rate, we insert a zero between each sample, and low-pass filter to clear the extended
    #part of the audio band. Any low-pass filter will do, as long as you pick one steep enough to get the job done,
    #removing the aliased copy without removing much of the existing signal band. Most often, a linear phase FIR filter is
    #used—performance is good at the relatively high cut-off frequency, phase is maintained, and we have good control over
    #its characteristics.

    #Downsampling

        #The process of reducing the sample rate—downsampling—is similar, except we low-pass filter first, to reduce the bandwidth,
        #then discard samples. The filter stage is essential, since the signal will alias if we try to fit it into a narrower
        #band without removing the portion that can’t be encoded at the lower rate. So, we set the filter cut-off to half the
        #new, lower sample rate, then simply discard every other sample for a 2:1 downsample ratio.
        #(Yes, the result will be slightly different depending on whether you discard the odd samples or even ones.
        #And no, it doesn’t matter, just as the exact sampling phase didn’t matter when you converted from analog to digital
        #in the first place.)

        #In general, the algorithm for resampling to a higher frequency is:
        #* maintain a 'cursor': a floating-point sample index, into the source sample
        #* for each sample, advance the cursor by (sourceSampleRate/targetSampleRate), which will be < 1.0
        #* interpolate data from the source sample according to the cursor position using the interpolation method of your choice; this will generally involve a polynomial using a small number of source samples around the cursor
        #For resampling to a lower frequency, the process is similar but the source sample should be lowpass filtered to attenuate everything above half the target sample rate before interpolation.

#++++++++++++++++++++++++++++++++++++++++++++++

#quando solo uma das inputs as outras também são enviadas. Ou seja, o solo está atuando apenas localmente. Faz sentido mudar isso?

#++++++++++++++++++++++++++++++++++++++++++++++

#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T11:05:00
#
#-------------------------------------------------

# Check if the config file exists
!include( ../Jamtaba-common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT       +=  gui  network widgets


TARGET = Jamtaba2
TEMPLATE = app

HEADERS += \
    ../src/audio/core/PortAudioDriver.h \
    ../src/recorder/JamRecorder.h \
    ../src/recorder/ReaperProjectGenerator.h \
    ../src/ninjam/protocol/ServerMessageParser.h \
    ../src/ninjam/protocol/ServerMessages.h \
    ../src/ninjam/protocol/ClientMessages.h \
    ../src/loginserver/natmap.h \
    ../src/audio/codec.h \
    ../src/midi/portmididriver.h \
    ../src/audio/Resampler.h \
    ../src/audio/vorbis/VorbisDecoder.h \
    ../src/audio/vorbis/VorbisEncoder.h \
    ../src/persistence/Settings.h \
    ../src/audio/vst/VstPlugin.h \
    ../src/audio/vst/vsthost.h \
    ../src/geo/WebIpToLocationResolver.h \
    ../src/StandAloneMainController.h \
    ../src/gui/MainWindowStandalone.h

SOURCES += \
    ../src/main.cpp \
    ../src/audio/core/PortAudioDriver.cpp \
    ../src/recorder/JamRecorder.cpp \
    ../src/recorder/ReaperProjectGenerator.cpp \
    ../src/loginserver/LoginService.cpp \
    ../src/loginserver/JsonUtils.cpp \
    ../src/ninjam/protocol/ServerMessages.cpp \
    ../src/ninjam/protocol/ClientMessages.cpp \
    ../src/ninjam/protocol/ServerMessageParser.cpp \
    ../src/ninjam/Server.cpp \
    ../src/audio/vst/PluginFinder.cpp \
    ../src/midi/portmididriver.cpp \
    ../src/audio/Resampler.cpp \
    ../src/audio/vorbis/VorbisDecoder.cpp \
    ../src/audio/SamplesBufferResampler.cpp \
    ../src/audio/samplesbufferrecorder.cpp \
    ../src/audio/vorbis/VorbisEncoder.cpp \
    ../src/persistence/Settings.cpp \
    ../src/audio/vst/VstPlugin.cpp \
    ../src/audio/vst/vsthost.cpp \
    ../src/geo/WebIpToLocationResolver.cpp \
    ../src/StandAloneMainController.cpp \
    ../src/gui/MainWindowStandalone.cpp

FORMS += \
    ../src/gui/pluginscandialog.ui \

INCLUDEPATH += $$PWD/../libs/includes/portaudio        \
               $$PWD/../libs/includes/portmidi         \
               $$PWD/../libs/includes/ogg              \
               $$PWD/../libs/includes/vorbis           \
               $$PWD/../libs/includes/minimp3          \
               #$$PWD/../libs/includes/libip2location   \

DEPENDPATH += $$PWD/../libs/includes/portaudio         \
               $$PWD/../libs/includes/portmidi         \
               $$PWD/../libs/includes/ogg              \
               $$PWD/../libs/includes/vorbis           \
               $$PWD/../libs/includes/minimp3          \
               #$$PWD/../libs/includes/libip2location   \


win32 {

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32  \

    VST_SDK_PATH = "E:/Jamtaba2/VST3 SDK/pluginterfaces/vst2.x/"

    !contains(QMAKE_TARGET.arch, x86_64) {
        message("x86 build") ## Windows x86 (32bit) specific build here
        LIBS_PATH = "win32-msvc"
    } else {
        message("x86_64 build") ## Windows x64 (64bit) specific build here
        LIBS_PATH = "win64-msvc"
    }

    CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/$$LIBS_PATH/ -lportaudio -lminimp3 -lportmidi -lvorbisfile -lvorbis -logg
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/$$LIBS_PATH/ -lportaudiod -lminimp3d -lportmidid -lvorbisfiled -lvorbisd -loggd

    RC_FILE = Jamtaba2.rc #windows icon
}

macx{
    message("Mac build")

    VST_SDK_PATH = "/Users/elieser/Desktop/VST3 SDK/pluginterfaces/vst2.x"

    macx-clang-32 {
        message("i386 build") ## mac 32bit specific build here
        LIBS_PATH = "mac32"
    } else {
        message("x86_64 build") ## mac 64bit specific build here
        LIBS_PATH = "mac64"
    }

    LIBS += -framework CoreAudio
    LIBS += -framework CoreMidi
    LIBS += -framework AudioToolbox
    LIBS += -framework AudioUnit
    LIBS += -framework CoreServices
    LIBS += -framework Carbon

    LIBS += -L$$PWD/libs/$$LIBS_PATH/ -lportaudio -lminimp3  -lportmidi -lvorbisfile -lvorbisenc -lvorbis -logg

    #mac osx doc icon
    ICON = Jamtaba.icns
}

INCLUDEPATH += $$VST_SDK_PATH  \



