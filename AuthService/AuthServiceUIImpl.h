#ifndef AUTH_SERVICE_UI_IMPL_H
#define AUTH_SERVICE_UI_IMPL_H
#include "AuthServiceUI.h"


class AuthServiceUIImpl : public AuthServiceUI
{
public:
	void scan() override;
	void updateProgress(const uint32_t current) override;
	void startScan(const uint32_t total) override;
private:

};

#endif //AUTH_SERVICE_UI_IMPL_H