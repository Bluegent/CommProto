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

struct ConfigNames
{
	static constexpr const char * userKey = "username";
	static constexpr const char * passKey = "hash";
	static constexpr const char * tokenKey = "tokens";
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
	return std::find(authTokens.begin(), authTokens.end(), token) != authTokens.end();
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
	for (const auto & token : *tokens)
	{
		if (token.isString()) {
			authTokens.emplace_back(token.toString());
		}
	}
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
		tokens.add(token);
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

void JSONLoginHandler::saveToken(const std::string& token)
{
	authTokens.emplace_back(token);
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

void JSONLoginHandler::setupKey()
{
	Poco::Crypto::CipherFactory& factory = Poco::Crypto::CipherFactory::defaultFactory();
	Poco::Crypto::CipherKey key = Poco::Crypto::CipherKey("aes-256-ecb", "shhitsasecret", "alsoasecret", 10, "sha1");
	cipher = factory.createCipher(key);
}
