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

    inline bool isEmpty() const
    {
        return groupedInputs.empty();
    }

    void addInput(Audio::LocalInputNode *input);

    inline int getIndex() const
    {
        return groupIndex;
    }

    void mixGroupedInputs(Audio::SamplesBuffer &out);

    void removeInput(Audio::LocalInputNode *input);

    int getMaxInputChannelsForEncoding() const;

    inline bool isTransmiting() const
    {
        return transmiting;
    }

    void setTransmitingStatus(bool transmiting);

private:
    int groupIndex;
    QList<Audio::LocalInputNode *> groupedInputs;
    bool transmiting;
};

}//namespace

#endif
