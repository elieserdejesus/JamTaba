#ifndef JAMTABA_TYPES
#define JAMTABA_TYPES

#include <QString>

namespace ninjam {

    namespace client {

        struct ChannelMetadata {
            QString name;
            bool voiceChatActivated = false;
        };
    }
}

#endif // #ifndef JAMTABA_TYPES
