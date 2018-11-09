#ifndef AF_SERVICE_H_
#define AF_SERVICE_H_

#ifdef SERVER

void AF_PushOperatorClientNumber(const int clNum);
int AF_PopOperatorClientNumber();

void AF_AddOperator(const int clNum);
void AF_RemoveOperator(const int clNum);
qboolean AF_IsClientOperator(const int clNum);

#endif

#endif


