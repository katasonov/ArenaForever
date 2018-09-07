#include "../qcommon/q_shared.h"

#ifdef SERVER

cvar_t	*af_aes_key;
cvar_t	*af_access_code;

#else

cvar_t	*af_player_data_enc;
cvar_t	*af_srv_access_code;

#endif // SERVER

void AF_Init(void)
{
#ifdef SERVER
	af_aes_key = Cvar_Get("af_aes_key", "DEFAULT_KEY", CVAR_INIT);
	af_access_code = Cvar_Get("af_access_code", "DEFAULT_ACCECC_CODE", CVAR_INIT);
#else
	af_player_data_enc = Cvar_Get("af_player_data_enc", "", CVAR_INIT | CVAR_USERINFO);
	af_srv_access_code = Cvar_Get("af_srv_access_code", "DEFAULT_ACCECC_CODE", CVAR_INIT | CVAR_USERINFO);
#endif
}

#ifdef SERVER

int AF_ClientHasAccess(char *access_code)
{
	if (strncmp(af_access_code->string, access_code, 16) == 0)
	{
		return 1;
	}
	return 0;
}

int AF_LoadUserInfo(char *encoded_string, char *userinfo)
{
	//TODO:
	return 1;
}

#endif