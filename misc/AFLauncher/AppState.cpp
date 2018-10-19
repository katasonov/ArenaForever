#include "AppState.h"

#include <mutex>

#include <map>

namespace {


	wstring		_authCode;
	wstring		_model;
	wstring		_sex;
	std::mutex	_mutex;

	wstring _vkAuthCheckCode; //used to get auth status from api.
}

AppState AppState::appState;


AppState::AppState()
{
	_authCode = L"";
	_model = L"assassin/default";
	_sex = L"male";

	//Load persistent data from file
	//TODO:
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

wstring AppState::GetVKOAuthUri()
{
	_mutex.lock();
	_vkAuthCheckCode = System::GenRandomWString(16);

#ifdef _DEBUG
	wstring url = WStrF(
		L"https://oauth.vk.com/authorize?client_id=6656867&display=page&" \
		L"redirect_uri=http://127.0.0.1:8889/api/v1/vklogin/clbk&scope=email&response_type=code&v=5.63&state=%s", _vkAuthCheckCode.c_str());
#else
	wstring url = WStrF(
		L"https://oauth.vk.com/authorize?client_id=6656867&display=page&" \
		L"redirect_uri=http://arenaforever.com/api/v1/vklogin/clbk&scope=email&response_type=code&v=5.63&state=%s", _vkAuthCheckCode.c_str());
#endif

	_mutex.unlock();

	return url;
}

wstring AppState::GetVKOAuthCheckStatusCode()
{
	_mutex.lock();
	wstring code = _vkAuthCheckCode;
	_mutex.unlock();
	return code;
}
