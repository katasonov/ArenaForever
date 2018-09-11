#include "../qcommon/q_shared.h"
#include "../AES/aes.h"

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
	af_aes_key = Cvar_Get("af_aes_key", "1234567890123456", CVAR_INIT);

	if (strlen(af_aes_key->string) != 16)
	{
		Com_Error(ERR_FATAL, "Size of af_aes_key must be 16 bytes");
	}

	af_access_code = Cvar_Get("af_access_code", "1234567890123456", CVAR_INIT);
#else
	af_player_data_enc = Cvar_Get("af_player_data_enc", "", CVAR_INIT | CVAR_MANAGED_USERINFO);
	af_srv_access_code = Cvar_Get("af_srv_access_code", "DEFAULT_ACCECC_CODE", CVAR_INIT | CVAR_MANAGED_USERINFO);
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

static uint8_t* hexstr2bytes(const char *ins, uint8_t *buff)
{
	size_t len = strlen(ins);
	//uint8_t *buff = (uint8_t*)malloc(len / 2);
	memset(buff, 0, BIG_INFO_STRING);
	int i;
	uint8_t val;
	for (i = 0; i < len / 2; i++)
	{
		if (i == 399)
			i = 399;
		sscanf(ins + i * 2, "%2hhx", &val);
		buff[i] = val;
	}
	return buff;
}

//userinfo - hex encoded string
int AF_LoadUserInfo(const char *cipher, char *userinfo)
{
	//TODO:
	uint8_t key[16];
	uint8_t buff[BIG_INFO_STRING];

	memset(buff, 0, sizeof(buff));

	size_t cipher_len = strlen(cipher);

	if (cipher_len < 1 || cipher_len % 16 != 0 || cipher_len + 1 > BIG_INFO_STRING)
		return 0;

	memcpy(key, af_aes_key->string, 16);	

	struct AES_ctx ctx;
	
	AES_init_ctx(&ctx, key);

	hexstr2bytes(cipher, buff);

	size_t blocks_n = cipher_len / 16;
	for (int i = 0; i < blocks_n; i++)
	{
		AES_ECB_decrypt(&ctx, buff + i * 16);
	}

	//userinfo = (char*)malloc(strlen(buff) + 1);
	strncpy(userinfo, buff, MAX_INFO_STRING);


	return 1;
}

#endif