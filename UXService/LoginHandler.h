#ifndef LOGIN_HANDLER_H
#define LOGIN_HANDLER_H
#include <vector>
#include <Poco/UUIDGenerator.h>
#include <Poco/Crypto/Cipher.h>
#include <memory>


class LoginHandler
{
public:
	virtual ~LoginHandler() = default;
	virtual bool validate(const std::string& user, const std::string & password) = 0;
	virtual bool validateToken(const std::string & token) = 0;
	virtual std::string generateToken() = 0;
	virtual void saveToken(const std::string & token, const bool longterm) = 0;
	virtual void saveUserDetails(const std::string & name, const std::string & pass) = 0;
	virtual bool validateSerial(const std::string & serial) = 0;
	virtual bool load() = 0;
	virtual bool save() = 0;
	virtual bool hasSetup() = 0;
};

using LoginHandlerHandle = std::shared_ptr<LoginHandler>;


struct UserDetails
{
	std::string username;
	std::string passwordHash;
};

struct TokenData
{
	std::string token;
	uint64_t expiryDate;
};

class JSONLoginHandler : public LoginHandler
{
public:
	JSONLoginHandler(const std::string & dbPath, const std::string & serial = { "AAAA-AAAA"});
	bool validate(const std::string& user, const std::string & password) override;
	bool validateToken(const std::string & token) override;
	std::string generateToken() override;
	bool load() override;
	bool hasSetup() override;
	bool save() override;
	~JSONLoginHandler() override;
	void saveToken(const std::string& token, const bool longterm) override;
	void saveUserDetails(const std::string & name, const std::string & pass) override;
	bool validateSerial(const std::string& serial) override;

private:
	void removeMoldyCookies();
	void setupKey();

	std::vector<TokenData> authTokens;
	std::string filePath;
	UserDetails user;
	bool isSetup;
	Poco::UUIDGenerator generator;
	Poco::Crypto::Cipher::Ptr cipher;
	std::string serial;
};

#endif //LOGIN_HANDLER_H
