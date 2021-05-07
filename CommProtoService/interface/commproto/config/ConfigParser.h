#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <rapidjson/document.h>
#include <vector>
#include <map>

namespace commproto
{
    namespace config
    {
        class ConfigParser
        {
            public:
                ConfigParser(const std::string & fileName);
                rapidjson::Document get() const;
			private:
				std::string file;
		};



		template <typename T>
		T getValueOrDefault(rapidjson::Document & doc, const char* const name, const T& defaultValue)
		{
			return defaultValue;
		}

		template <>
		inline bool getValueOrDefault(rapidjson::Document & doc, const char* const name, const bool& defaultValue)
		{
            if(doc.HasParseError() || !doc.IsObject())
            {
                return defaultValue;
            }
            if (!doc.HasMember(name))
            {
                return defaultValue;
            }
            if (!doc[name].IsBool())
			{
				return defaultValue;
			}
			return doc[name].GetBool();
		}

		template <>
		inline int32_t getValueOrDefault(rapidjson::Document & doc, const char* const name, const int32_t& defaultValue)
        {
            if(doc.HasParseError() || !doc.IsObject())
            {
                return defaultValue;
            }

            if (!doc.HasMember(name))
			{
				return defaultValue;
			}

            if(!doc[name].IsInt())
            {
                return defaultValue;
            }

			return doc[name].GetInt();
		}

	    inline const char* getValueOrDefault(rapidjson::Document & doc, const char* const name, const char * defaultValue)
		{
            if(doc.HasParseError() || !doc.IsObject())
            {
                return defaultValue;
            }
            if (!doc.HasMember(name))
            {
                return defaultValue;
            }
            if (!doc[name].IsString())
			{
				return defaultValue;
			}
			return doc[name].GetString();
		}


		template <>
		inline std::vector<std::string> getValueOrDefault(rapidjson::Document & doc, const char* const name, const std::vector<std::string>& defaultValue)
		{
			if (doc.HasParseError() || !doc.IsObject())
			{
				return defaultValue;
			}
			if (!doc.HasMember(name))
			{
				return defaultValue;
			}
			if (!doc[name].IsArray())
			{
				return defaultValue;
			}
			auto arr = doc[name].GetArray();
			if(!arr.Size())
			{
				return defaultValue;
			}
			std::vector<std::string> result;
			for(const auto &entry : arr)
			{
				if(!entry.IsString())
				{
					return defaultValue;
				}
				result.emplace_back(entry.GetString());
			}
			return result;
			
		}

		template <>
		inline std::map<std::string,std::string> getValueOrDefault(rapidjson::Document & doc, const char* const name, const  std::map<std::string, std::string>& defaultValue)
		{
			if (doc.HasParseError() || !doc.IsObject())
			{
				return defaultValue;
			}
			if (!doc.HasMember(name))
			{
				return defaultValue;
			}
			if (!doc[name].IsObject())
			{
				return defaultValue;
			}
			std::map<std::string, std::string> result;

			auto obj = doc[name].GetObject();

			for(const auto & entry :obj)
			{
				if(entry.name.IsString() &&  entry.value.IsString())
				{
					result.emplace(entry.name.GetString(), entry.value.GetString());
				}
			}

			return result;
			
		}

    }
}


#endif // CONFIG_PARSER_H
