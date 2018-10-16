#include "AppState.h"

#include <mutex>

namespace {


	wstring		_authCode;
	wstring		_model;
	wstring		_sex;
	std::mutex	_mutex;


}

AppState AppState::appState;


AppState::AppState()
{
	_authCode = L"";
	_model = L"assassin/default";
	_sex = L"male";
}

void AppState::SetAuthCode(const wstring &appCode)
{
	_mutex.lock();
	_authCode = appCode;
	_mutex.unlock();
}

wstring AppState::GetAuthCode()
{
	_mutex.lock();
	wstring appCode = _authCode;
	_mutex.unlock();
	return appCode;
}

void AppState::SetPlayerModelName(const wstring &model)
{
	_mutex.lock();
	_model = model;
	_mutex.unlock();
}

wstring AppState::GetPlayerModelName()
{
	_mutex.lock();
	wstring model = _model;
	_mutex.unlock();
	return model;
}

void AppState::SetPlayerSex(const wstring &sex)
{
	_mutex.lock();
	_sex = sex;
	_mutex.unlock();
}

wstring AppState::GetPlayerSex()
{
	_mutex.lock();
	wstring sex = _sex;
	_mutex.unlock();
	return sex;
}
