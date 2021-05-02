#include "../interface/DynamicModuleImpl.h"
#include <commproto/logger/Logging.h>


namespace commproto
{
	namespace plugin
	{
		DynamicModuleImpl::DynamicModuleImpl()
			: dllInstance(nullptr)
			, path{}
		{
		}

		DynamicModuleImpl::~DynamicModuleImpl()
		{
			DynamicModuleImpl::close();
		}

		bool DynamicModuleImpl::open(const std::string & path_)
		{
			if (dllInstance)
			{
				return false;
			}
			path = getLibraryName(path_);

			dllInstance = LoadLibraryA(path.c_str());
			if (!dllInstance)
			{
				LOG_ERROR("Could not open dynamic module from path %s", path.c_str());
				return false;
			}
			return true;
		}

		void DynamicModuleImpl::close()
		{
			if (!dllInstance)
			{
				return;
			}
			FreeLibrary(dllInstance);
			dllInstance = nullptr;
		}

		void* DynamicModuleImpl::getFunction(const std::string& name)
		{
			return GetProcAddress(dllInstance, name.c_str());
		}

		std::string DynamicModuleImpl::getPath() const
		{
			return path;
		}

		const std::string libSuffix = std::string{ ".dll" };

		std::string DynamicModuleImpl::getLibraryName(const std::string& name)
		{
			return name + libSuffix;
		}
	}
}
