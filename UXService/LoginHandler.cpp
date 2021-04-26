#include "LoginHandler.h"
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include "FileLoader.h"
#include <Poco/Path.h>
#include <fstream>
#include <Poco/Crypto/CipherFactory.h>
#include <Poco/UUIDGenerator.h>
#include <poco/Crypto/RSAKey.h>
#include <poco/Crypto/Cipher.h>
#include <poco/Crypto/CipherKey.h>
#include <chrono>

struct ConfigNames
{
	static constexpr const char * userKey = "username";
	static constexpr const char * passKey = "hash";
	static constexpr const char * tokenKey = "tokens";
	static constexpr const char * tokenStrKey = "token";
	static constexpr const char * expiry = "expiry";
	static constexpr const uint64_t tenDaysMs = 10 * 24 * 60 * 60 * 1000;
	static constexpr const uint64_t halfADay = 12 * 60 * 60 * 1000;
};



JSONLoginHandler::JSONLoginHandler(const std::string& dbPath, const std::string& serial_)
	: isSetup{ false }
	, filePath{ dbPath }
	, serial{ serial_ }
{
}


bool JSONLoginHandler::validate(const std::string& username, const std::string& password)
{
	return user.username == username && user.passwordHash == cipher->encryptString(password, Poco::Crypto::Cipher::ENC_BASE64);
}

bool JSONLoginHandler::validateToken(const std::string& token)
{
	removeMoldyCookies();
	for (const auto & tok : authTokens)
	{
		if (tok.token == token)
		{
			return true;
		}
	}
	return false;
}

std::string JSONLoginHandler::generateToken()
{
	return  cipher->encryptString(generator.createOne().toString(), Poco::Crypto::Cipher::ENC_BASE64);
}

bool JSONLoginHandler::load()
{
	setupKey();
	Poco::Path path(filePath);
	std::string contents = FileLoader::getFileContents(path.absolute().toString());
	if (contents.empty())
	{
		return false;
	}

	Poco::JSON::Parser parser;
	Poco::Dynamic::Var result = parser.parse(contents);
	Poco::JSON::Object::Ptr root = result.extract<Poco::JSON::Object::Ptr>();
	if (root->has(ConfigNames::userKey) && root->get(ConfigNames::userKey).isString() && root->has(ConfigNames::passKey) && root->get(ConfigNames::passKey).isString())
	{
		user.username = root->get(ConfigNames::userKey).toString();
		user.passwordHash = root->get(ConfigNames::passKey).toString();
		isSetup = true;
	}

	if (!root->has(ConfigNames::tokenKey))
	{
		return true;
	}

	if (!root->get(ConfigNames::tokenKey).isArray())
	{
		return true;
	}

	auto tokens = root->getArray(ConfigNames::tokenKey);
	for (uint32_t index = 0; index < root->size(); ++index)
	{
		if (!tokens->isObject(index))
		{
			continue;
		}
		auto tokenObj = tokens->getObject(index);
		if (!tokenObj->has(ConfigNames::expiry) || !tokenObj->has(ConfigNames::tokenStrKey))
		{
			continue;
		}

		TokenData data;
		data.token = tokenObj->get(ConfigNames::tokenStrKey).toString();
		data.expiryDate = tokenObj->get(ConfigNames::expiry).extract<int64_t>();
		authTokens.push_back(data);
	}
	removeMoldyCookies();
	save();
	return true;

}

bool JSONLoginHandler::hasSetup()
{
	return isSetup;
}

bool JSONLoginHandler::save()
{
	if (!isSetup)
	{
		return false;
	}
	std::ofstream file(filePath);
	if (!file.is_open())
	{
		return false;
	}

	Poco::JSON::Object root;
	root.set(ConfigNames::userKey, user.username);
	root.set(ConfigNames::passKey, user.passwordHash);
	Poco::JSON::Array tokens;
	for (const auto & token : authTokens)
	{
		Poco::JSON::Object tok;
		tok.set(ConfigNames::tokenStrKey, token.token);
		tok.set(ConfigNames::expiry, token.expiryDate);
		tokens.add(tok);
	}
	root.set(ConfigNames::tokenKey, tokens);

	root.stringify(file);
	file.close();
	return true;
}

JSONLoginHandler::~JSONLoginHandler()
{
	JSONLoginHandler::save();
}

void JSONLoginHandler::saveToken(const std::string& token, const bool longterm)
{
	TokenData data;
	data.token = token;
	if (longterm) 
	{
		data.expiryDate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + ConfigNames::tenDaysMs;
	}
	else
	{
		data.expiryDate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + ConfigNames::halfADay;
	}
	authTokens.emplace_back(data);
	save();
}

void JSONLoginHandler::saveUserDetails(const std::string& name, const std::string& pass)
{
	isSetup = true;
	user.username = name;
	user.passwordHash = cipher->encryptString(pass, Poco::Crypto::Cipher::ENC_BASE64);
	save();
}

bool JSONLoginHandler::validateSerial(const std::string& serial_)
{
	return serial == serial_;
}

void JSONLoginHandler::removeMoldyCookies()
{
	uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	for (auto it = authTokens.begin(); it != authTokens.end();)
	{
		if (it->expiryDate < now)
		{
			it = authTokens.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void JSONLoginHandler::setupKey()
{
	Poco::Crypto::CipherFactory& factory = Poco::Crypto::CipherFactory::defaultFactory();
	Poco::Crypto::CipherKey key = Poco::Crypto::CipherKey("aes-256-ecb", "shhitsasecret", "alsoasecret", 10, "sha1");
	cipher = factory.createCipher(key);
}
