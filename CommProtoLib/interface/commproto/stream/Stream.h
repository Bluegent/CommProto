#ifndef COMMPROTO_STREAM_H
#define COMMPROTO_STREAM_H

#include <commproto/common/Common.h>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace commproto 
{
    namespace stream 
    {
        class Stream;
        using StreamHandle = std::shared_ptr<Stream>;
        class Stream {
        public:
            virtual ~Stream() = default;
            //sendBytes a byte array, blocking
            virtual int32_t sendBytes(const Message& message) = 0;

            //block the thread until the stream receives [size] bytes
            virtual int32_t receive(Message& message, const uint32_t size) = 0;

            //poll the stream to see how many bytes are buffered
            virtual int32_t available() = 0;

            //block the thread until single byte of data can be read from the stream
            virtual int readByte() = 0;

            //block the thread until single byte of data can be sent to the stream
            virtual int sendByte(const char byte) = 0;

            //check connection state
            virtual bool connected() = 0;

            //close the connection
            virtual void shutdown() = 0;
        };

    } // namespace core
} // namespace stream

#endif //COMMPROTO_STREAM_H
