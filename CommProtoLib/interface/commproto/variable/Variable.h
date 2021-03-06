#ifndef COMMPROTO_VALUE_H
#define COMMPROTO_VALUE_H

#include <commproto/messages/MessageBase.h>
#include <commproto/variable/VariableBase.h>
#include <commproto/variable/Context.h>
#include <commproto/variable/VariableMessage.h>
#include <commproto/parser/ByteStream.h>
#include <sstream>
#include "commproto/logger/Logging.h"

namespace commproto
{
	namespace variable
	{
		class VariableSerializer
		{

		public:
			template <typename T>
			Message serialize(messages::MessageBase && data) const
			{
				VariableMessageTemplated<T>& value = static_cast<VariableMessageTemplated<T>&>(data);
				parser::ByteStream stream;
				stream.writeHeader(data);
				stream.write(value.valueType);
				stream.write(value.index);
				stream.write(value.value);
				return stream.getStream();

			}
		};

		template <typename T, ValueType  UnderlyingType>
		class Variable : public VariableBase
		{
		public:
			Variable(const ContextHandle& ctx, const T& value_ = T{}, const bool sendUniqueOnly = true);

			//used to set the value by the owner, notifying the context that something has changed
			Variable& operator=(const T& value_);

			std::string toString() override;

			Message serialize() override;

			~Variable() = default;

			const T& get() const
			{
				return value;
			}

			//used to set the value externally by a handler, notifying the context that an update of the value occured
			void set(const T& value_)
			{
				if (value == value_)
				{
					return;
				}
				value = value_;
				context->notifyIn(index);
			}

		private:
			bool uniqueOnly;
			T value;
			VariableSerializer serializer;
		};

		template <typename T, ValueType UnderlyingType>
		Variable<T, UnderlyingType>::Variable(const ContextHandle& ctx, const T& value_, const bool sendUniqueOnly)
			: VariableBase{ UnderlyingType, ctx }
			, uniqueOnly(sendUniqueOnly)
			, value{ value_ }
		{
		}

		template <typename T, ValueType UnderlyingType>
		Variable<T, UnderlyingType>& Variable<T, UnderlyingType>::operator=(const T& value_)
		{
			if (uniqueOnly && value == value_)
			{
				return *this;
			}
			value = value_;
			context->notifyOut(index);
			return *this;
		}

		template <typename T, ValueType UnderlyingType>
		std::string Variable<T, UnderlyingType>::toString()
		{
			std::stringstream ss;
			ss << value << "(id:" << index << ")";
			return ss.str();
		}

		template <>
		inline std::string Variable<bool, ValueType::bool8>::toString()
		{
			std::stringstream ss;
			ss << (value?"True":"False") << "(id:" << index << ")";
			return ss.str();
		}

		template <typename T, ValueType UnderlyingType>
		Message Variable<T, UnderlyingType>::serialize()
		{
			return serializer.serialize<T>(
				std::move(VariableMessageTemplated<T>(context->getVarTypeId(), UnderlyingType, index, value)));
		}

		using IntegerVariable = Variable<uint32_t, ValueType::integer32>;
		using IntegerVariableHandle = std::shared_ptr<IntegerVariable>;

		using StringVariable = Variable<std::string, ValueType::string>;
		using StringVariableHandle = std::shared_ptr<StringVariable>;

		using RealVariable = Variable<float, ValueType::real32>;
		using RealVariableHandle = std::shared_ptr<RealVariable>;

		using BoolVariable = Variable<bool, ValueType::bool8>;
		using BoolVariableHandle = std::shared_ptr<BoolVariable>;

	}
}

#endif
