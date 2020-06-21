#include "RtMidiDriver.h"
#include "RtMidi.h"

#include "MidiMessage.h"
#include "log/Logging.h"

using midi::RtMidiDriver;
using midi::MidiMessage;

RtMidiDriver::RtMidiDriver(const QList<bool> &deviceStatuses){

    qCDebug(jtMidi) << "Initializing rtmidi...";

    QList<bool> statuses(deviceStatuses);
    int maxInputDevices = getMaxInputDevices();

    qCDebug(jtMidi) << "MIDI INPUT DEVICES FOUND idx:" << maxInputDevices;

    if(statuses.size() < maxInputDevices){
        int itemsToAdd = maxInputDevices - statuses.size();
        for (int i = 0; i < itemsToAdd; ++i) {
            statuses.append(true);
        }
    }
    setInputDevicesStatus(statuses);

    int maxOutputDevices = getMaxOutputDevices();

    qCDebug(jtMidi) << "MIDI OUTPUT DEVICES FOUND idx:" << maxOutputDevices;

//    if(statuses.size() < maxInputDevices){
//        int itemsToAdd = maxInputDevices - statuses.size();
//        for (int i = 0; i < itemsToAdd; ++i) {
//            statuses.append(true);
//        }
//    }
//    setInputDevicesStatus(statuses);

    qCDebug(jtMidi) << "rtmidi initialized!";

}

void RtMidiDriver::setInputDevicesStatus(const QList<bool> &statuses){
    qCDebug(jtMidi) << "Setting input devices status in RtMidiDriver";

    release();

    int midiDevicesCount = getMaxInputDevices();
    QList<bool> validStatuses;
    for (int i = 0; i < midiDevicesCount; ++i) {
        if (i < statuses.count())
            validStatuses.append(statuses.at(i));
        else
            validStatuses.append(true); //new connected devices are enabled by default
    }

    MidiDriver::setInputDevicesStatus(validStatuses);
    for (int s = 0; s < validStatuses.size(); ++s) {
        midiInStreams.append(new RtMidiIn());
    }
}

void RtMidiDriver::start(const QList<bool> &deviceStatuses){
    qCDebug(jtMidi) << "Starting RtMidiDriver";

    setInputDevicesStatus(deviceStatuses);

    if(!hasInputDevices() && !hasOutputDevices()){
        return;
    }
    stop();

    for(int deviceIndex=0; deviceIndex < inputDevicesEnabledStatuses.size(); deviceIndex++) {
        if(deviceIndex < midiInStreams.size()){
            RtMidiIn* stream = midiInStreams.at(deviceIndex);
            if(stream && inputDevicesEnabledStatuses.at(deviceIndex)){//device is globally enabled?
                if(!stream->isPortOpen()){
                    try{
                        qCInfo(jtMidi) << "Starting MIDI Input in " << QString::fromStdString(stream->getPortName(deviceIndex));
                        stream->ignoreTypes();// ignoring sysex, miditime and midi sense messages
                        stream->openPort(deviceIndex);
                    }
                    catch(RtMidiError &e){
                        qCritical() << "Error opening midi input port " << QString::fromStdString(e.getMessage());
                    }
                }
                else{
                    qCritical() << "Port " << QString::fromStdString(stream->getPortName(deviceIndex)) << " already opened!";
                }
            }
        }
    }

    for(int deviceIndex=0; deviceIndex < getMaxOutputDevices()/*inputDevicesEnabledStatuses.size()*/; deviceIndex++) {
        if(deviceIndex < midiOutStreams.size()){
            RtMidiOut* stream = midiOutStreams.at(deviceIndex);
            if(stream /*&& inputDevicesEnabledStatuses.at(deviceIndex)*/){//device is globally enabled?
                if(!stream->isPortOpen()){
                    try{
                        qCInfo(jtMidi) << "Starting MIDI Output in " << QString::fromStdString(stream->getPortName(deviceIndex));
                        stream->openPort(deviceIndex);
                    }
                    catch(RtMidiError &e){
                        qCritical() << "Error opening midi output port " << QString::fromStdString(e.getMessage());
                    }
                }
                else{
                    qCritical() << "Port " << QString::fromStdString(stream->getPortName(deviceIndex)) << " already opened!";
                }
            }
        }
    }
}

void RtMidiDriver::stop(){
    qCDebug(jtMidi) << "Stopping RtMidiDriver (closing " << midiInStreams.size() << " streams)";
    foreach (RtMidiIn* stream, midiInStreams) {
        if(stream){
            stream->closePort();
        }
    }
    foreach (RtMidiOut* stream, midiOutStreams) {
        if(stream){
            stream->closePort();
        }
    }
   qCDebug(jtMidi) << "RtMidiDriver stoped!";
}

void RtMidiDriver::release(){

    if (midiInStreams.isEmpty() && midiOutStreams.isEmpty())
        return;

    qCDebug(jtMidi) << "Releasing RtMidiDriver";

    foreach (RtMidiIn* stream, midiInStreams) {
        if(stream){
            if(stream->isPortOpen()){
                stream->closePort();
            }
            delete stream;
        }
    }
    foreach (RtMidiOut* stream, midiOutStreams) {
        if(stream){
            if(stream->isPortOpen()){
                stream->closePort();
            }
            delete stream;
        }
    }
    midiInStreams.clear();
}

QString RtMidiDriver::getInputDeviceName(uint index) const{
    RtMidiIn rtMidi;
    if(index < rtMidi.getPortCount())
        return QString::fromStdString(rtMidi.getPortName(index));
    return "";
}

QString RtMidiDriver::getOutputDeviceName(uint index) const{
    RtMidiOut rtMidi;
    if(index < rtMidi.getPortCount())
        return QString::fromStdString(rtMidi.getPortName(index));
    return "";
}

void RtMidiDriver::sendClockStart() const{
    const std::vector<unsigned char> message = {250};
    for(auto stream : midiOutStreams) {
        stream->sendMessage(&message);
    }
}

void RtMidiDriver::sendClockTick() const{
    const std::vector<unsigned char> message = {248};
    for(auto stream : midiOutStreams) {
        stream->sendMessage(&message);
    }
}

void RtMidiDriver::consumeMessagesFromStream(RtMidiIn *stream, int deviceIndex, std::vector<midi::MidiMessage> &outBuffer)
{
    //qCDebug(jtMidi) << "consuming messages from stream - RtMidiDriver";
    std::vector<unsigned char> messageBytes;
    do{
        messageBytes.clear();
        stream->getMessage(&messageBytes);
        if (messageBytes.size() == 3) { // Jamtaba is handling only the 3 bytes common midi messages. Uncommon midi messages will be ignored.
            outBuffer.push_back(midi::MidiMessage::fromVector(messageBytes, deviceIndex));
        }
        else{
            if (!messageBytes.empty())
                qWarning() << "A midi message containing " << messageBytes.size() << " bytes was received!";
        }
    }
    while(!messageBytes.empty());
}

std::vector<MidiMessage> RtMidiDriver::getBuffer()
{
    std::vector<midi::MidiMessage> buffer;
    int deviceIndex = 0;
    for (auto stream : midiInStreams) {
        consumeMessagesFromStream(stream, deviceIndex, buffer);
        deviceIndex++;
    }
    return buffer;
}

bool RtMidiDriver::hasInputDevices() const{
    return getMaxInputDevices() > 0;
}

bool RtMidiDriver::hasOutputDevices() const{
    return getMaxOutputDevices() > 0;
}

int RtMidiDriver::getMaxInputDevices() const{
    try
    {
        RtMidiIn rtMidi;
        return rtMidi.getPortCount();
    }
    catch(const RtMidiError &e) {
        qCDebug(jtMidi) << QString::fromStdString(e.getMessage());
    }
    catch(...) {
        qCritical() << "ERROR NO MIDI!";
    }

    return 0;
}

int RtMidiDriver::getMaxOutputDevices() const{
    try
    {
        RtMidiOut rtMidiOut;
        return rtMidiOut.getPortCount();
    }
    catch(const RtMidiError &e) {
        qCDebug(jtMidi) << QString::fromStdString(e.getMessage());
    }
    catch(...) {
        qCritical() << "ERROR NO MIDI!";
    }

    return 0;
}

RtMidiDriver::~RtMidiDriver(){
    //release();
}
