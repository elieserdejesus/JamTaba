#dar feedback quando o usuário escolher noInput. Deixar a pista esmaecida seria legal.
#Usar setEnabled não funcionou porque desabilita inclusive o combo de selação, o que
#impossibilita que o usuário volte a deixar a pista ativa.

#salvar as entradas selecionadas para a pista local (salvar primeiro índice e quantidade de canais).
#Deixar para pensar em como salvar quando  já tiver decidido como serão as múltiplas entradas

#    Nome do canal
#        range sub1
#        range sub2
#    Nome do canal 2
#        ranges


#implementar a conversão de vários canais para um só. Isso será necessário quando o
#usuário entrar em um server que aceita menos canais do que ele tinha criado. Pegar
#os subchannels dos canais adicionais e fazer um merge com o último canal válido. Por
#exemplo, se o server aceita 2 canais e eu estou com 3 então ele vai fazer um merge 2+3


#implementar os subchannels
#por enquanto comentei o código do NinjamController que encodava, depois tenho que pensar em como vou encodar vários canais
#no método que recria os encoders eu estou dando o migué e estou recriando apenas um

#implementar xmit




#preciso mudar a persistência dos nomes dos channels para um array

#permitir a criação de subcanais é importante. Eu posso estar em um server que permite apenas um canal mas estou usando vários
#subcanais. O canal pai tem um nome, os sub são apenas locais, são mixados e enviados como o canal pai. Acho que essa é uma boa
#arquitetura para as entradas mas preciso pensar bem sobre as características do canal pai, acho que os canais filhos são os que
#eu já tenho

#também preciso tratar a situação onde o usuário está usando midi como entrada e o driver midi é alterado nas preferencias




#como vou permitir vários devices midi? Pelo que vi no portaudio.h o único jeito seria abrir vários streams midi, um para cada device.
#acho que é melhor deixar isso para mais adiante, por que também terei que mudar a forma como estou lendo as mensagens midi e passando
#elas adiante. Como cada pista local vai poder ler de um midi device diferente eu acho que a solução seria criar um MidiBuffer e disponibilizá-lo
#para o processReplacing como eu fiz com o SamplesBuffer. Esse MidiBuffer teria vários canais, cada canal contendo as mensagens midi de um device
#diferente

#deu pau com o kontakt também. Só está carregando plugins pequenos?

#deu pau quando tentei colocar o Addictive Drums

#abri a aplicação com a fast track e deu pau porque 192 é uma SR inválida. Tenho que pedir as SR válidas para cada device.
#já aproveitar para pedir os buffer sizes


#consegui entrar no jamtaba com a fast track desligada. O canal apareceu como "not connected"

#não consegui votar para trocar bpi

#preciso testar nome de usuário com caracter especial para ver se o utf está funcionando

#não está lembrando das entradas estereo com a fast track

#acho que poderia simplificar a criação do menu de plugins usando a mesma ideia
#que usei para gerar o menu das inputs da pista local. Como a pista local tem
#uma referência para o mainController eu posso pedir para ele os PluginDEscriptors

#no ninjamJamController estou recriando o tempInBuffer em cada callback. Otimizar isso.

# a mensagem de crowded está errada?


#será que o resampler não acumula as amostras? Eu estou achando que sim. Se
#eu passar out.frameLenght para ele mas o buffer resampleado tiver out.lenght + 1 samples
#eu acho que ele vai guardar essa última amostra no buffer interno, mas eu teria
#que sair do loop do resampler exatamente em out.lenght

#separar o carregamento do plugin VST da instância. No momento uma instância é criada e depois é que o plugin é carregado. Pra mim
#isso é um bad design


#o metronome precisa ficar na tela o tempo todo, ele é muito importante. Se ele ficar junto com as pistas locais ele sumirá
#quando a seção com as pistas locais for contraída

#Preciso mudar a cor de fundo da pistas de acordo com o tipo. Pistas locais de uma cor, metronomo de outra, pistas ninjam de outra.
#Talvez seja uma boa hora para usar HSV e ter variações

#tirar spacers dos títulos das seções?

#ver o construtor do mainController, acho que comentei a inserção do roomStreamer na lista de nodes, por isso não está tocando os streams
#acho que quando fico alternando entre os streams das salas não está funcionando muito bem, parece que o botão ficou pressionado.
#acho que o stream do ninjamer não está rolando

# MIDI funcionando, mas se seleciono o midi da FAST track e depois volto para o SPS ele não funciona mais. Testar com o controlador AKAI também para ver

#dialogo de IO do midi - testar novamente, ver se a seleção do midi device está funcionando

#não consegui resolver o bug que acontece quando as pistas são removidas, por hora apenas comentei a linha que delete as pistas no NinjamJamRoomController. Ou seja, a memoria não está sendo liberada.
#Agora estou usando vector<float> para guardar as amostras, ver o que acontece.

# Quando botei o reverb depois do B4 ouvi o reverb na entrada do mic mas não no B4, o encadeamento tem problema
#Na verdade preciso repensar isso, não faz sentido ter midi e áudio ao mesmo tempo em uma pista

# não estou chamando o startProcess nos VSTs, isso pode bugar VSTs que utilizam

# drummix multi deu problema na mixagem dos canais, acho que só consegui ouvir o bumbo e o vazamendo das outras peças

#buga tudo se não tem conexão com a internet

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

#BUGS relacionados com o diálogo de audioIO:
#1 - quando seleciono as entradas sPDIF da fast track a aplicação encerra
#2 - quando seleciono saídas que não são 1 e 2 dá um crash também.
#3 - Com a fast track quando seleciono as entradas e simplesmente volto para a tela de audio IO o valor do segundo combo está bugado.
#4 - preciso testar com a fonte da fast track para ver se os outros canais estão realmente funcionando


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
    src/jamtaba/persistence/ConfigStore.h \
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
    src/jamtaba/NinjamJamRoomController.h \
    src/jamtaba/gui/MetronomeTrackView.h \
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
    src/jamtaba/gui/TrackIOPanel.h \
    src/jamtaba/audio/vorbis/VorbisEncoder.h \
    src/jamtaba/gui/LocalTrackGroupView.h \
    src/jamtaba/gui/Highligther.h


SOURCES += \
    $$MAIN \
#    nvwa/debug_new.cpp \
#-----------------------------------------
    src/jamtaba/persistence/ConfigStore.cpp \
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
    src/jamtaba/NinjamJamRoomController.cpp \
    src/jamtaba/gui/MetronomeTrackView.cpp \
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
    src/jamtaba/gui/TrackIoPanel.cpp \
    src/jamtaba/audio/vorbis/VorbisEncoder.cpp \
    src/jamtaba/gui/LocalTrackGroupView.cpp \
    src/jamtaba/gui/Highligther.cpp

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
    src/jamtaba/gui/TrackIOPanel.ui \
    src/jamtaba/gui/LocalTrackGroupView.ui


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
               $$PWD/libs/includes/libresample  \
               $$PWD/libs/includes/minimp3      \
               $$PWD/libs/includes/libmaxmind   \


win32: LIBS +=  -L$$PWD/libs/win32-mingw/ -lportaudio -lportmidi -lvorbisfile -lvorbis -lvorbisenc -logg -lminimp3 -lmaxminddb -lresample \

win32: LIBS +=  -lwinmm     \
                -lole32     \
                -lws2_32    \

RESOURCES += src/jamtaba/resources/jamtaba.qrc

#INCLUDEPATH += $$PWD/libs/includes/portaudio
#DEPENDPATH += $$PWD/libs/includes/portaudio
