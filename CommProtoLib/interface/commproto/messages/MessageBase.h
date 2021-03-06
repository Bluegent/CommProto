#ifndef COMMPROTO_TYPEBASE_H
#define  COMMPROTO_TYPEBASE_H
#include <stdint.h>
#include <string>
#include <vector>
#include <commproto/messages/SenderMaping.h>


//used to define boilerplate rule of 5 since we want to use std::move with types derived from TypeBase
#define BP_TYPE_DEFAULTS(Class)						\
	Class(const Class& other) = default;			\
	Class(Class&& other) = default;					\
	Class& operator=(const Class& other) = default;	\
	Class& operator=(Class&&other) = default;		\
	virtual ~Class() = default;						\


namespace commproto
{
	namespace messages
	{
		template <typename T>
		uint32_t sizeOf(const T& value)
		{
			return sizeof(value);
		}

		template <>
		inline uint32_t sizeOf(const std::string & value)
		{
			return sizeof(uint32_t) //first we write the size of the string
				+ value.size(); //then the amount of chars
		}


		template <typename T>
		uint32_t sizeOf(const std::vector<T>& values)
		{
			uint32_t size = sizeof(uint32_t); //first we write the size of the vector
			for (const auto & elem : values)
			{
				size += sizeOf(elem); //then add size of each element
			}
			return size;
		}

		class MessageBase
		{
		public:
			MessageBase(const uint32_t type_) : type{ type_ }, senderId{ SenderMapping::getId() } {};

			BP_TYPE_DEFAULTS(MessageBase);

			uint32_t type;
			uint32_t senderId;
			virtual uint32_t getSize() const
			{
				return sizeof(type) + sizeof(senderId);
			}

		};
	}
}

#endif //COMMPROTO_TYPEBASE_H