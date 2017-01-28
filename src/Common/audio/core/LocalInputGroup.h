#ifndef _LOCAL_INPUT_GROUP_H_
#define _LOCAL_INPUT_GROUP_H_

#include <QList>

namespace Audio {

class LocalInputNode;
class SamplesBuffer;

class LocalInputGroup
{
public:
    LocalInputGroup(int groupIndex, Audio::LocalInputNode *firstInput);
    ~LocalInputGroup();

    bool isEmpty() const;

    void addInputNode(Audio::LocalInputNode *input);

    int getIndex() const;

    void mixGroupedInputs(Audio::SamplesBuffer &out);

    void removeInput(Audio::LocalInputNode *input);

    int getMaxInputChannelsForEncoding() const;

    bool isTransmiting() const;

    void setTransmitingStatus(bool transmiting);

    Audio::LocalInputNode *getInputNode(quint8 index) const;

private:
    int groupIndex;
    QList<Audio::LocalInputNode *> groupedInputs;
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

}//namespace

#endif
