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
				if (updates.find(id) != updates.end())
				{
					return;
				}
				updates.emplace(id, true);
			}

			void UpdateTracker::remove(const uint32_t id)
			{
				auto it = updates.find(id);
				if (it == updates.end())
				{
					return;
				}
				updates.erase(it);
			}

			bool UpdateTracker::hasUpdates()
			{
				for (auto update : updates)
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
				std::map<uint32_t, bool>::iterator it = updates.find(controlId);
				if (it == updates.end())
				{
					return;
				}
				it->second = update;
			}

			bool UpdateTracker::hasUpdate(const uint32_t controlId)
			{
				std::map<uint32_t, bool>::iterator it = updates.find(controlId);
				if (it == updates.end())
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
