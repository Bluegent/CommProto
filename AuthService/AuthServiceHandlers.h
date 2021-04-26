#ifndef AUTH_SERVICE_HANDLERS_H
#define AUTH_SERVICE_HANDLERS_H


#include "AuthService.h"
#include <commproto/parser/Handler.h>
#include <commproto/device/AuthChains.h>



class DeviceReqHandler : public commproto::parser::Handler
{
public:
	DeviceReqHandler(const AuthServiceHandle& service_);
	void handle(commproto::messages::MessageBase&& data) override;
private:
	AuthServiceHandle service;
};


class ScanFinishedHandler : public commproto::parser::Handler
{
public:
	ScanFinishedHandler(const AuthServiceHandle & service_);
	void handle(commproto::messages::MessageBase&& data) override;
private:
	AuthServiceHandle service;
};


class KeepAliveHandler : public commproto::parser::Handler
{
public:
	KeepAliveHandler(const AuthServiceHandle& service_);
	void handle(commproto::messages::MessageBase&& data) override;
private:
	AuthServiceHandle service;
};


class ScanStartHandler : public commproto::parser::Handler
{
public:
	ScanStartHandler(const AuthServiceHandle& service_) : service{ service_ } {};
	void handle(commproto::messages::MessageBase&& data) override;
private:
	AuthServiceHandle service;
};

class ScanProgressHandler : public commproto::parser::Handler
{
public:
	ScanProgressHandler(const AuthServiceHandle& service_) : service{ service_ } {};
	void handle(commproto::messages::MessageBase&& data) override;
private:
	AuthServiceHandle service;
};


#endif //AUTH_SERVICE_HANDLERS_H