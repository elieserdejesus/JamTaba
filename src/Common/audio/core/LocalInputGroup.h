#ifndef _LOCAL_INPUT_GROUP_H_
#define _LOCAL_INPUT_GROUP_H_

#include <QList>

namespace audio {

class LocalInputNode;
class SamplesBuffer;

class LocalInputGroup
{

public:
    LocalInputGroup(int groupIndex, audio::LocalInputNode *firstInput);
    ~LocalInputGroup();

    bool isEmpty() const;

    void addInputNode(audio::LocalInputNode *input);

    int getIndex() const;

    void mixGroupedInputs(audio::SamplesBuffer &out);

    void removeInput(audio::LocalInputNode *input);

    int getMaxInputChannelsForEncoding() const;

    bool isTransmiting() const;

    void setTransmitingStatus(bool transmiting);

    audio::LocalInputNode *getInputNode(quint8 index) const;

private:
    int groupIndex;
    QList<audio::LocalInputNode *> groupedInputs;
    bool transmiting;
};

inline bool LocalInputGroup::isTransmiting() const
{
    return transmiting;
}

inline int LocalInputGroup::getIndex() const
{
    return groupIndex;
}

inline bool LocalInputGroup::isEmpty() const
{
    return groupedInputs.empty();
}

} //namespace

#endif
