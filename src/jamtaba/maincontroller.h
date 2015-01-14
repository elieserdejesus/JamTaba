#pragma once

class AudioDriver;
class AudioDriverListener;

class MainController
{
public:
    MainController();
    ~MainController();

    void start();
    void stop();
    AudioDriver* getAudioDriver() const;

private:
    AudioDriver* audioDriver;
    AudioDriverListener* audioDriverListener;
};

