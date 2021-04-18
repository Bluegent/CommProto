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
				, shown{ true }
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

			bool isEnabled() const
			{
				return enabled;
			}

			void setDisplayState(const bool shown_)
			{
				shown = shown_;
			}

			bool isVisible() const
			{
				return shown;
			}

		protected:
			const std::string name;
			const uint32_t id;
			bool enabled;
			bool shown;
		};

	}
}


#endif // CONTROL_H