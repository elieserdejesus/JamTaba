#parece que o stream das salas não está rolando mesmo. Vi que tinha problema na assinatura dos métodos.
    #não consegui testar os streams adequadamente ainda
#acho que quando fico alternando entre os streams das salas não está funcionando muito bem, parece que o botão ficou pressionado.
#acho que o stream do ninjamer não está rolando

#Não estava deletando as tracks de input, por isso o plugin vst não era deletado, o destrutor
    #nunca era invocado e a DLL não era descarregada, gerando um erro no fechamento.
# tirei o comentário do código que deleta a pista, tenho que ver se ainda tem o bug quando
    #deleto um canal no Reaninjam.


#Prioridades para um primeiro release
    #Ver o usode memória para não queimar o filme
    #auto updater

#preciso testar nome de usuário com caracter especial para ver se o utf está funcionando

#não está lembrando das entradas estereo com a fast track

# a mensagem de crowded está errada?

#não consegui resolver o bug que acontece quando as pistas são removidas, por hora apenas comentei a linha que delete as pistas no NinjamJamRoomController. Ou seja, a memoria não está sendo liberada.
#Agora estou usando vector<float> para guardar as amostras, ver o que acontece.

# Quando botei o reverb depois do B4 ouvi o reverb na entrada do mic mas não no B4, o encadeamento tem problema

#BUGS relacionados com o diálogo de audioIO:
#1 - quando seleciono as entradas sPDIF da fast track a aplicação encerra
#3 - Com a fast track quando seleciono as entradas e simplesmente volto para a tela de audio IO o valor do segundo combo está bugado.


#tocar em algumas Jams para ver se está rolando
#-------------------------------- PRIMEIRO RELEASE ----------------------------------------

#se desligo a fast enquanto estou com o dialogo de preferencias aberto dá pau em seguida

#buga tudo se não tem conexão com a internet

#se ligo a fast enquanto o Jamtaba está aberto ela não aparece na lista. Algum tipo de cache na portaudio?

#separar o carregamento do plugin VST da instância. No momento uma instância é criada e depois é que o plugin é carregado. Pra mim
#isso é um bad design

#VST
    #agora que estou usando todos os canais de saida do plugin tive um
        #memory corruption quando fechei o kontakt, mas foi esporárico, provavelmente
        #alguma coisa relacionada com a thread que está removendo o plugin
    #preciso mixar as várias saídas do plugin para um par estereo

#nomes grandes estragam os nome dos canais nas entradas, os nomes dos canais ninjam, etc. Uma AutoElidedQLabel seria legal.

#adicionar diretórios default para scanear plugins no win 64

#dar feedback quando plugin for bypassado, deixar label mais escura - Não consegui fazer isso, a propriedade existe no objeto, mas o CSS não está funcionando, desisti

#o botão clear cache não dá nenhum feedback. Seria legal mostrar a lista de plugins
#que estão na cache e depois limpar essa lista


#feedback para o carregamento de plugin pode ser interessante, o Addicitve drum leva uma eternidade.
    #Talvez eu tenha que carregar os plugins só depois que a aplicação já estiver visível, caso
    #contrário demora demais para ver a tela principal se a lista de plugins para carregar
    #for grande e os plugins forem pesados.

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


#parece que quando ativo o encoding (entradas setadas) a memória não para de crescer. Fica mais evidente com dois canais.

#no ninjamJamController estou recriando o tempInBuffer em cada callback. Otimizar isso.

#exibir uma mensagem no chat quando um usuário sai da sala

#Entrei em uma sala com uns 5 caras e estava usando 25:% da minha CPU.
#Entrei na mesma sala usando o Reaninjam e não chegou a 1% da minha CPU. Mas depois eu descobri que se não estiver encodando
#o reaninjam usa pouca CPU. O bicho pega mesmo quando tem que encodar. Mesmo assim acho que estou usando muita CPU.
    #Quanto de cpu usa sem entrar em uma sala?
    #Quanto de CPU usa para encoder?
    #E os decoders, acrescentam muita coisa na CPU? Posso criar um monte de canais no reaninjam e ver o que acontece.

#O consumo de memória está aumentando sem parar e não consegui achar o erro. Preciso de um Valgrind. Mas parece que o problema é no encoding. Testar isso.



#leak - quando deletar um encode do map de encoders? Como saber lá no NinjamController que o usuário está com um canal a menos?

#estava bugando o parser da lista de servers públicos no servidor, mas vi que no reaper também bugou. O problema é que o Jamtaba nem abre se tem esse problema, dá um erro no servidor.


#estou usando ícone de alto falante para representar inputs, é estranho



#++++++++++++++++++++++++++++++++++++++++++++++
#GAz deu a ideia de fazer um translate usando o site do google translate e HTML scrapping.

#Comentei com o Marcello sobre a ideia de criar um segundo chat para mensagens privadas.

#quando solo uma das inputs as outras também são enviadas. Ou seja, o solo está atuando apenas localmente. Faz sentido mudar isso?

#usuário kn00t perguntou sobre um log do chat. Pode ser interessante. Talvez ele só queira ver as mensagens mais antigas de uma sessão, e não ver mensagens de outras sessões.
#++++++++++++++++++++++++++++++++++++++++++++++

#Preciso mudar a cor de fundo da pistas de acordo com o tipo. Pistas locais de uma cor, metronomo de outra, pistas ninjam de outra.
#Talvez seja uma boa hora para usar HSV e ter variações


#Resampling
    #Ainda tem um probleminha nos início de alguns intervalos, mas só acontece em
    #algumas combinações de SR e como está já está bem aceitável. Eu não estou corrigindo
    #a perda de amostras que vai acontecendo aos poucos, por isso chega no final do intervalo
    #pode dar algumas amostras de diferença.

#tem um bug quando o bpi está em 4

#chat ninjam
    #problema nos caracteres especiais - tenho que testar se o envio está correto e depois testar a recepção

# feature - botão para ouvir o stream dos server e ver como está a mix geral

#audo update: https://wiki.qt.io/Qt-Installer-Framework

#mapa com os jogadores? É possível?

#PLUGINS NATIVOS
    # mostrar plugins nativos
    # Melhorar o visual do Delay, os LineEdit estão grandes demais
    # Seria interessante possibilitar que os parâmetros do delay sejam alterados através dos lineEdit

#Magnus e Doublebass agradeceram pelo esforço e disseram que ter um canal de backing track é muito importante.

# - Coisas legais para implementar: fazer um plugin de delay para iniciar a saga dos plugins nativos do Jamtaba
# - O stream do ninjamer.com não é mono como achei que era, mas está dando problema


#quando trocar de bpi ver se ainda é possível usar a mesma acentuação procurando pelo valor
#antigo na nova lista


#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T11:05:00
#
#-------------------------------------------------

QT       +=  gui  network

#DEFINES += QT_NO_CAST_FROM_ASCII
#DEFINES += QT_NO_CAST_TO_ASCII

QMAKE_CXXFLAGS += -D _CRT_SECURE_NO_WARNINGS #-Wno-unused-parameter

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Jamtaba2
TEMPLATE = app

MAIN = src/jamtaba/main.cpp
#MAIN = src/jamtaba/ninjam/main.cpp


HEADERS += \
    #nvwa/debug_new.h \
    #--------------------------------
#    #--------------------------------
    src/jamtaba/audio/core/AudioDriver.h \
    src/jamtaba/audio/core/AudioNode.h \
    src/jamtaba/audio/core/AudioMixer.h \
    src/jamtaba/audio/core/PortAudioDriver.h \
#    #--------------------------------
    src/jamtaba/gui/widgets/PeakMeter.h \
    src/jamtaba/gui/widgets/WavePeakPanel.h \
#    #--------------------------------
    src/jamtaba/loginserver/LoginService.h \
    src/jamtaba/loginserver/JsonUtils.h \
#    #--------------------------------
    src/jamtaba/MainController.h \
    src/jamtaba/JamtabaFactory.h \
#    #--------------------------------
    src/jamtaba/ninjam/protocol/ServerMessageParser.h \
    src/jamtaba/ninjam/protocol/ServerMessages.h \
    src/jamtaba/ninjam/protocol/ClientMessages.h \
    src/jamtaba/ninjam/User.h \
    src/jamtaba/ninjam/Service.h \
    src/jamtaba/ninjam/Server.h \
#    #--------------------------------
    src/jamtaba/loginserver/natmap.h \
    src/jamtaba/audio/RoomStreamerNode.h \
    src/jamtaba/audio/codec.h \
    src/jamtaba/gui/LocalTrackView.h \
    src/jamtaba/gui/JamRoomViewPanel.h \
    src/jamtaba/gui/MainFrame.h \
    src/jamtaba/gui/FxPanel.h \
    src/jamtaba/gui/FxPanelItem.h \
    src/jamtaba/audio/core/plugins.h \
    src/jamtaba/gui/plugins/guis.h \
    src/jamtaba/audio/vst/PluginFinder.h \
    src/jamtaba/audio/vst/VstPlugin.h \
    src/jamtaba/audio/vst/vsthost.h \
    src/jamtaba/midi/MidiDriver.h \
    src/jamtaba/midi/portmididriver.h \
    src/jamtaba/gui/pluginscandialog.h \
    src/jamtaba/gui/PreferencesDialog.h \
    src/jamtaba/gui/NinjamRoomWindow.h \
    src/jamtaba/gui/BaseTrackView.h \
    src/jamtaba/audio/NinjamTrackNode.h \
    src/jamtaba/gui/NinjamTrackView.h \
    src/jamtaba/audio/MetronomeTrackNode.h \
    src/jamtaba/gui/NinjamPanel.h \
    src/jamtaba/gui/FancyProgressDisplay.h \
    src/jamtaba/audio/Resampler.h \
    src/jamtaba/audio/vorbis/VorbisDecoder.h \
    src/jamtaba/ninjam/UserChannel.h \
    src/jamtaba/audio/core/SamplesBuffer.h \
    src/jamtaba/gui/BusyDialog.h \
    src/jamtaba/audio/core/AudioPeak.h \
    src/jamtaba/geo/IpToLocationResolver.h \
    src/jamtaba/gui/ChatPanel.h \
    src/jamtaba/gui/ChatMessagePanel.h \
    src/jamtaba/audio/SamplesBufferResampler.h \
    src/jamtaba/audio/samplesbufferrecorder.h \
    src/jamtaba/audio/vorbis/VorbisEncoder.h \
    src/jamtaba/gui/Highligther.h \
    src/jamtaba/persistence/Settings.h \
    src/jamtaba/Utils.h \
    src/jamtaba/gui/TrackGroupView.h \
    src/jamtaba/gui/LocalTrackGroupView.h \
    src/jamtaba/NinjamController.h \
    src/jamtaba/gui/IntervalProgressDisplay.h


SOURCES += \
    $$MAIN \
#    nvwa/debug_new.cpp \
#-----------------------------------------
##------------------------------------------------
    src/jamtaba/audio/core/AudioDriver.cpp \
    src/jamtaba/audio/core/AudioNode.cpp \
    src/jamtaba/audio/core/AudioMixer.cpp \
    src/jamtaba/audio/core/PortAudioDriver.cpp \
    src/jamtaba/audio/RoomStreamerNode.cpp \
##------------------------------------------------
    src/jamtaba/gui/widgets/PeakMeter.cpp \
    src/jamtaba/gui/widgets/WavePeakPanel.cpp \
##------------------------------------------------
    src/jamtaba/JamtabaFactory.cpp \
    src/jamtaba/MainController.cpp \
##------------------------------------------------
    src/jamtaba/loginserver/LoginService.cpp \
    src/jamtaba/loginserver/JsonUtils.cpp \
##------------------------------------------------
    src/jamtaba/ninjam/protocol/ServerMessages.cpp \
    src/jamtaba/ninjam/protocol/ClientMessages.cpp \
    src/jamtaba/ninjam/protocol/ServerMessageParser.cpp \
    src/jamtaba/ninjam/Server.cpp \
    src/jamtaba/ninjam/Service.cpp \
    src/jamtaba/ninjam/User.cpp \
    src/jamtaba/gui/LocalTrackView.cpp \
    src/jamtaba/gui/FxPanel.cpp \
    src/jamtaba/gui/FxPanelItem.cpp \
    src/jamtaba/audio/core/plugins.cpp \
    src/jamtaba/audio/codec.cpp \
    src/jamtaba/gui/plugins/guis.cpp \
    src/jamtaba/gui/JamRoomViewPanel.cpp \
    src/jamtaba/gui/MainFrame.cpp \
    src/jamtaba/audio/vst/PluginFinder.cpp \
    src/jamtaba/audio/vst/VstPlugin.cpp \
    src/jamtaba/audio/vst/vsthost.cpp \
    src/jamtaba/midi/MidiDriver.cpp \
    src/jamtaba/gui/PreferencesDialog.cpp \
    src/jamtaba/gui/PluginScanDialog.cpp \
    src/jamtaba/midi/PortMidiDriver.cpp \
    src/jamtaba/gui/NinjamRoomWindow.cpp \
    src/jamtaba/gui/BaseTrackView.cpp \
    src/jamtaba/audio/NinjamTrackNode.cpp \
    src/jamtaba/gui/NinjamTrackView.cpp \
    src/jamtaba/audio/MetronomeTrackNode.cpp \
    src/jamtaba/gui/NinjamPanel.cpp \
    src/jamtaba/gui/FancyProgressDisplay.cpp \
    src/jamtaba/audio/Resampler.cpp \
    src/jamtaba/audio/vorbis/VorbisDecoder.cpp \
    src/jamtaba/ninjam/UserChannel.cpp \
    src/jamtaba/audio/core/SamplesBuffer.cpp \
    src/jamtaba/gui/BusyDialog.cpp \
    src/jamtaba/audio/core/AudioPeak.cpp \
    src/jamtaba/geo/IpToLocationResolver.cpp \
    src/jamtaba/gui/ChatPanel.cpp \
    src/jamtaba/gui/ChatMessagePanel.cpp \
    src/jamtaba/audio/SamplesBufferResampler.cpp \
    src/jamtaba/audio/samplesbufferrecorder.cpp \
    src/jamtaba/audio/vorbis/VorbisEncoder.cpp \
    src/jamtaba/gui/Highligther.cpp \
    src/jamtaba/persistence/Settings.cpp \
    src/jamtaba/gui/TrackGroupView.cpp \
    src/jamtaba/gui/LocalTrackGroupView.cpp \
    src/jamtaba/NinjamController.cpp \
    src/jamtaba/gui/IntervalProgressDisplay.cpp

FORMS += \
    src/jamtaba/gui/PreferencesDialog.ui \
    src/jamtaba/gui/PluginScanDialog.ui \
    src/jamtaba/gui/MainFrame.ui \
    src/jamtaba/gui/JamRoomViewPanel.ui \
    src/jamtaba/gui/NinjamRoomWindow.ui \
    src/jamtaba/gui/BaseTrackView.ui \
    src/jamtaba/gui/NinjamPanel.ui \
    src/jamtaba/gui/BusyDialog.ui \
    src/jamtaba/gui/ChatPanel.ui \
    src/jamtaba/gui/ChatMessagePanel.ui \
    src/jamtaba/gui/TrackGroupView.ui


#macx: LIBPATH += /Users/Eliesr/Qt5.4.0/5.4/clang_64/lib \
#win32:LIBPATH += C:/Qt/Qt5.4.0/Tools/mingw491_32/i686-w64-mingw32/lib/ \
#win32:LIBPATH += C:\Qt\Qt5.4.2\5.4\msvc2013\lib

VST_SDK_PATH = "D:/Documents/Estudos/ComputacaoMusical/Jamtaba2/VST3_SDK/pluginterfaces/vst2.x/"


INCLUDEPATH += src/jamtaba/gui                  \
               src/jamtaba/gui/widgets          \
               $$VST_SDK_PATH                   \
               $$PWD/libs/includes/portaudio    \
               $$PWD/libs/includes/portmidi     \
               $$PWD/libs/includes/ogg          \
               $$PWD/libs/includes/vorbis       \
               $$PWD/libs/includes/minimp3      \
               $$PWD/libs/includes/libmaxmind   \


win32: LIBS +=  -L$$PWD/libs/win32-mingw/ -lportaudio -lportmidi -lvorbisfile -lvorbis -lvorbisenc -logg -lminimp3 -lmaxminddb  \

win32: LIBS +=  -lwinmm     \
                -lole32     \
                -lws2_32    \

RESOURCES += src/jamtaba/resources/jamtaba.qrc

#INCLUDEPATH += $$PWD/libs/includes/portaudio
#DEPENDPATH += $$PWD/libs/includes/portaudio
