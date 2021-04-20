#include "UpdateTracker.h"
#include <chrono>

namespace commproto
{
    namespace control
    {
        namespace ux
        {
			UpdateTracker::UpdateTracker()
			{
				setAccessed();
			}

			void UpdateTracker::add(const uint32_t id)
			{
				if (controlUpdates.find(id) != controlUpdates.end())
				{
					return;
				}
				controlUpdates.emplace(id, true);
			}

			void UpdateTracker::remove(const uint32_t id)
			{
				auto it = controlUpdates.find(id);
				if (it == controlUpdates.end())
				{
					return;
				}
				controlUpdates.erase(it);
			}

			bool UpdateTracker::hasUpdates()
			{
				for (auto update : controlUpdates)
				{
					if (update.second)
					{
						return true;
					}
				}
				return false;
			}

			void UpdateTracker::setUpdate(const uint32_t controlId, const bool update)
			{
				std::map<uint32_t, bool>::iterator it = controlUpdates.find(controlId);
				if (it == controlUpdates.end())
				{
					return;
				}
				it->second = update;
			}

			bool UpdateTracker::hasUpdate(const uint32_t controlId)
			{
				std::map<uint32_t, bool>::iterator it = controlUpdates.find(controlId);
				if (it == controlUpdates.end())
				{
					return false;
				}
				return it->second;
			}

			void UpdateTracker::setAccessed()
			{
				lastAccesssed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}

			uint32_t UpdateTracker::getTimeSinceLastAccess() const
			{
				uint32_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				return now - lastAccesssed;
			}
        }
    }
}
