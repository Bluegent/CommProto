#include "../interface/DynamicModuleImpl.h"
#include <commproto/logger/Logging.h>
#include <dlfcn.h>

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

            dllInstance = dlopen(path.c_str(),RTLD_LAZY);
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
            dlclose(dllInstance);
            dllInstance = nullptr;
        }

        void* DynamicModuleImpl::getFunction(const std::string& name)
        {
            return dlsym(dllInstance, name.c_str());
        }

        std::string DynamicModuleImpl::getPath() const
        {
            return path;
        }

        const std::string libSuffix = std::string{ ".so" };
        const std::string libPrefix = std::string{ "lib" };

        std::string DynamicModuleImpl::getLibraryName(const std::string& name)
        {
            return libPrefix + name + libSuffix;
        }
    }
}
