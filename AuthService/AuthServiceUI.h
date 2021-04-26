#ifndef AUTH_SERVICE_UI_H
#define AUTH_SERVICE_UI_H
#include <stdint.h>


class AuthServiceUI
{
public:
	virtual ~AuthServiceUI() = default;
	virtual void scan() = 0;
	virtual void updateProgress(const uint32_t current) = 0;
	virtual void startScan(const uint32_t total) = 0;

};

#endif //AUTH_SERVICE_UI_H