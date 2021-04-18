#ifndef CONTROL_H
#define CONTROL_H

#include <string>

namespace commproto
{
	namespace control {

		class BaseControl
		{
		public:
			BaseControl(const std::string& name_, const uint32_t id_)
				: name{ name_ }
				, id{ id_ }
				, enabled{ true }
			{
			}

			std::string getName() const
			{
				return name;
			}

			uint32_t getId() const
			{
				return id;
			}

			void setState(const bool enabled_)
			{
				enabled = enabled_;
			}

		protected:
			const std::string name;
			const uint32_t id;
			bool enabled;
		};

	}
}


#endif // CONTROL_H