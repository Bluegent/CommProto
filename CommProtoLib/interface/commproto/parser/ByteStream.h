#ifndef COMMPROTO_BYTE_STREAM_H
#define COMMPROTO_BYTE_STREAM_H

#include <vector>
#include <string>
#include <string.h>
#include <commproto/common/Common.h>
#include <commproto/messages/MessageBase.h>

namespace commproto
{
	namespace parser
	{
		class ByteStream
		{
		public:
			ByteStream() : position(0) {}

			ByteStream(const Message & stream_) : stream(stream_), position(0) {}

			template <typename T>
			bool read(T& value);

			template <typename T>
			void write(const T& value);

			bool read(void* buf, const uint32_t size);
			void write(const void* buf, const uint32_t size);

			template <typename T>
			bool read(std::vector<T> & vec);

			template <typename T>
			void write(const std::vector<T> & vec);

			bool skip(uint32_t bytes);
				
			const Message & getStream() const
			{
				return stream;
			}

			void writeHeader(messages::MessageBase & data)
			{
				write(data.getSize());
				write(data.type);
				write(data.senderId);
			}

		private:
			Message stream;
			uint32_t position;
		};



		// read and write plain values
		template <typename  T>
		bool ByteStream::read(T& value)
		{
			uint32_t valueSize = sizeof(value);
			if (position + valueSize > stream.size())
			{
				return false;
			}
			memcpy(&value, reinterpret_cast<T*>(stream.data() + position), valueSize);
			position += valueSize;
			return true;
		}

		template <typename T>
		void ByteStream::write(const T& value)
		{
			const uint8_t* valuePtr = reinterpret_cast<const uint8_t*>(&value);
			uint32_t valueSize = sizeof(value);
			position += valueSize;
			stream.insert(stream.end(), valuePtr, valuePtr + valueSize);
		}

		//read and write std::string
		template <>
		inline bool ByteStream::read(std::string & value) {
			uint32_t size = 0;
			bool res = read(size);
			if (!res)
			{
				return false;
			}

			//won't work if the system doesn't store strings continuously in memory
			value.resize(size);
			value.reserve(size);
			return read(&value[0], size);
		}


		template <>
		inline void ByteStream::write(const std::string & value) {
			write<uint32_t>(value.size());
			write(value.c_str(), value.size());
		}

		//read and write raw arrays
		inline bool ByteStream::read(void* buf, const uint32_t size)
		{
			if(!size)
			{
				return true;
			}
			if (!buf || position + size > stream.size())
			{
				return false;
			}

			memcpy(buf, stream.data() + position, size);
			position += size;
			return true;
		}

		inline void ByteStream::write(const void* buf, const uint32_t size)
		{
			if (!size || !buf)
			{
				return;
			}

			const uint8_t* bufPtr = reinterpret_cast<const uint8_t*>(buf);
			stream.insert(stream.end(), bufPtr, bufPtr + size);
			position += size;
		}

		inline bool ByteStream::skip(uint32_t bytes)
		{
			if (position + bytes >= stream.size()) {
				return false;
			}
			position += bytes;
			return true;
		}

		//read and write generic vectors
		template <typename T>
		bool ByteStream::read(std::vector<T>& vec)
		{
			uint32_t size = 0;
			bool res = read(size);
			if (!res)
			{
				return false;
			}

			vec.reserve(size);
			for (uint32_t index = 0; index < size; ++index)
			{
				T temp;
				if (!read(temp))
				{
					return false;
				}
				vec.emplace_back(temp);
			}
			return true;
		}

		template <typename T>
		void ByteStream::write(const std::vector<T>& vec)
		{
			write<uint32_t>(vec.size());
			for (const T& element : vec)
			{
				write(element);
			}
		}


	}
}

#endif //COMMPROTO_BYTE_STREAM_H
