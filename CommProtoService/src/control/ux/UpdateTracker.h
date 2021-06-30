#ifndef UPDATE_TRACKER_H
#define UPDATE_TRACKER_H
#include <map>
#include <memory>
#include <vector>
#include <string>

namespace commproto
{
	namespace control
	{
		namespace ux
		{
			class UpdateTracker
			{
			public:
				UpdateTracker();
				void add(const uint32_t);
				void remove(const uint32_t);
				void clearRemoved();
				bool hasUpdates();   	
				void setUpdate(const uint32_t controlId, const bool update);
				bool hasUpdate(const uint32_t controlId);
				const std::vector<uint32_t> & getRemoved() const;			
				void setAccessed();
				uint32_t getTimeSinceLastAccess() const;
				std::string name;
			private:
				std::map<uint32_t, bool> updates;
				uint32_t lastAccesssed;
				std::vector<uint32_t> removed;
			};
			using UpdateTrackerHandle = std::shared_ptr<UpdateTracker>;
		}
	}
}


#endif// UPDATE_TRACKER_H