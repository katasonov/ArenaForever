/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
// cg_syscalls.c -- this file is only included when building a dll
// cg_syscalls.asm is included instead when building a qvm
#ifdef Q3_VM
#error "Do not use in VM build"
#endif

#include "cg_local.h"

static intptr_t (QDECL *syscall)( intptr_t arg, ... ) = (intptr_t (QDECL *)( intptr_t, ...))-1;


#ifndef USE_STATIC_MODS
Q_EXPORT void dllEntry(intptr_t(QDECL *syscallptr)(intptr_t arg, ...)) {
	syscall = syscallptr;
}
#else
//Used for static linkage
void cgameModuleEntry(intptr_t(QDECL *syscallptr)(intptr_t arg, ...)) {
	syscall = syscallptr;
}
#endif // USE_STATIC_MODS

int cg_PASSFLOAT( float x ) {
	floatint_t fi;
	fi.f = x;
	return fi.i;
}

void	cg_trapPrint( const char *fmt ) {
	syscall( CG_PRINT, fmt );
}

void cg_trapError(const char *fmt)
{
	syscall(CG_ERROR, fmt);
	// shut up GCC warning about returning functions, because we know better
	exit(1);
}

int		cg_trapMilliseconds( void ) {
	return syscall( CG_MILLISECONDS ); 
}

void	cg_trapCvar_Register( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags ) {
	syscall( CG_CVAR_REGISTER, vmCvar, varName, defaultValue, flags );
}

void	cg_trapCvar_Update( vmCvar_t *vmCvar ) {
	syscall( CG_CVAR_UPDATE, vmCvar );
}

void	cg_trapCvar_Set( const char *var_name, const char *value ) {
	syscall( CG_CVAR_SET, var_name, value );
}

void cg_trapCvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize ) {
	syscall( CG_CVAR_VARIABLESTRINGBUFFER, var_name, buffer, bufsize );
}

int		cg_trapArgc( void ) {
	return syscall( CG_ARGC );
}

void	cg_trapArgv( int n, char *buffer, int bufferLength ) {
	syscall( CG_ARGV, n, buffer, bufferLength );
}

void	cg_trapArgs( char *buffer, int bufferLength ) {
	syscall( CG_ARGS, buffer, bufferLength );
}

int		cg_trapFS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode ) {
	return syscall( CG_FS_FOPENFILE, qpath, f, mode );
}

void	cg_trapFS_Read( void *buffer, int len, fileHandle_t f ) {
	syscall( CG_FS_READ, buffer, len, f );
}

void	cg_trapFS_Write( const void *buffer, int len, fileHandle_t f ) {
	syscall( CG_FS_WRITE, buffer, len, f );
}

void	cg_trapFS_FCloseFile( fileHandle_t f ) {
	syscall( CG_FS_FCLOSEFILE, f );
}

int cg_trapFS_Seek( fileHandle_t f, long offset, int origin ) {
	return syscall( CG_FS_SEEK, f, offset, origin );
}

void	cg_trapSendConsoleCommand( const char *text ) {
	syscall( CG_SENDCONSOLECOMMAND, text );
}

void	cg_trapAddCommand( const char *cmdName ) {
	syscall( CG_ADDCOMMAND, cmdName );
}

void	cg_trapRemoveCommand( const char *cmdName ) {
	syscall( CG_REMOVECOMMAND, cmdName );
}

void	cg_trapSendClientCommand( const char *s ) {
	syscall( CG_SENDCLIENTCOMMAND, s );
}

void	cg_trapUpdateScreen( void ) {
	syscall( CG_UPDATESCREEN );
}

void	cg_trapCM_LoadMap( const char *mapname ) {
	syscall( CG_CM_LOADMAP, mapname );
}

int		cg_trapCM_NumInlineModels( void ) {
	return syscall( CG_CM_NUMINLINEMODELS );
}

clipHandle_t cg_trapCM_InlineModel( int index ) {
	return syscall( CG_CM_INLINEMODEL, index );
}

clipHandle_t cg_trapCM_TempBoxModel( const vec3_t mins, const vec3_t maxs ) {
	return syscall( CG_CM_TEMPBOXMODEL, mins, maxs );
}

clipHandle_t cg_trapCM_TempCapsuleModel( const vec3_t mins, const vec3_t maxs ) {
	return syscall( CG_CM_TEMPCAPSULEMODEL, mins, maxs );
}

int		cg_trapCM_PointContents( const vec3_t p, clipHandle_t model ) {
	return syscall( CG_CM_POINTCONTENTS, p, model );
}

int		cg_trapCM_TransformedPointContents( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles ) {
	return syscall( CG_CM_TRANSFORMEDPOINTCONTENTS, p, model, origin, angles );
}

void	cg_trapCM_BoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
						  const vec3_t mins, const vec3_t maxs,
						  clipHandle_t model, int brushmask ) {
	syscall( CG_CM_BOXTRACE, results, start, end, mins, maxs, model, brushmask );
}

void	cg_trapCM_CapsuleTrace( trace_t *results, const vec3_t start, const vec3_t end,
						  const vec3_t mins, const vec3_t maxs,
						  clipHandle_t model, int brushmask ) {
	syscall( CG_CM_CAPSULETRACE, results, start, end, mins, maxs, model, brushmask );
}

void	cg_trapCM_TransformedBoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
						  const vec3_t mins, const vec3_t maxs,
						  clipHandle_t model, int brushmask,
						  const vec3_t origin, const vec3_t angles ) {
	syscall( CG_CM_TRANSFORMEDBOXTRACE, results, start, end, mins, maxs, model, brushmask, origin, angles );
}

void	cg_trapCM_TransformedCapsuleTrace( trace_t *results, const vec3_t start, const vec3_t end,
						  const vec3_t mins, const vec3_t maxs,
						  clipHandle_t model, int brushmask,
						  const vec3_t origin, const vec3_t angles ) {
	syscall( CG_CM_TRANSFORMEDCAPSULETRACE, results, start, end, mins, maxs, model, brushmask, origin, angles );
}

int		cg_trapCM_MarkFragments( int numPoints, const vec3_t *points, 
				const vec3_t projection,
				int maxPoints, vec3_t pointBuffer,
				int maxFragments, markFragment_t *fragmentBuffer ) {
	return syscall( CG_CM_MARKFRAGMENTS, numPoints, points, projection, maxPoints, pointBuffer, maxFragments, fragmentBuffer );
}

void	cg_trapS_StartSound( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx ) {
	syscall( CG_S_STARTSOUND, origin, entityNum, entchannel, sfx );
}

void	cg_trapS_StartLocalSound( sfxHandle_t sfx, int channelNum ) {
	syscall( CG_S_STARTLOCALSOUND, sfx, channelNum );
}

void	cg_trapS_ClearLoopingSounds( qboolean killall ) {
	syscall( CG_S_CLEARLOOPINGSOUNDS, killall );
}

void	cg_trapS_AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx ) {
	syscall( CG_S_ADDLOOPINGSOUND, entityNum, origin, velocity, sfx );
}

void	cg_trapS_AddRealLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx ) {
	syscall( CG_S_ADDREALLOOPINGSOUND, entityNum, origin, velocity, sfx );
}

void	cg_trapS_StopLoopingSound( int entityNum ) {
	syscall( CG_S_STOPLOOPINGSOUND, entityNum );
}

void	cg_trapS_UpdateEntityPosition( int entityNum, const vec3_t origin ) {
	syscall( CG_S_UPDATEENTITYPOSITION, entityNum, origin );
}

void	cg_trapS_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater ) {
	syscall( CG_S_RESPATIALIZE, entityNum, origin, axis, inwater );
}

sfxHandle_t	cg_trapS_RegisterSound( const char *sample, qboolean compressed ) {
	return syscall( CG_S_REGISTERSOUND, sample, compressed );
}

void	cg_trapS_StartBackgroundTrack( const char *intro, const char *loop ) {
	syscall( CG_S_STARTBACKGROUNDTRACK, intro, loop );
}

void	cg_trapR_LoadWorldMap( const char *mapname ) {
	syscall( CG_R_LOADWORLDMAP, mapname );
}

qhandle_t cg_trapR_RegisterModel( const char *name ) {
	return syscall( CG_R_REGISTERMODEL, name );
}

qhandle_t cg_trapR_RegisterSkin( const char *name ) {
	return syscall( CG_R_REGISTERSKIN, name );
}

qhandle_t cg_trapR_RegisterShader( const char *name ) {
	return syscall( CG_R_REGISTERSHADER, name );
}

qhandle_t cg_trapR_RegisterShaderNoMip( const char *name ) {
	return syscall( CG_R_REGISTERSHADERNOMIP, name );
}

void cg_trapR_RegisterFont(const char *fontName, int pointSize, fontInfo_t *font) {
	syscall(CG_R_REGISTERFONT, fontName, pointSize, font );
}

void	cg_trapR_ClearScene( void ) {
	syscall( CG_R_CLEARSCENE );
}

void	cg_trapR_AddRefEntityToScene( const refEntity_t *re ) {
	syscall( CG_R_ADDREFENTITYTOSCENE, re );
}

void	cg_trapR_AddPolyToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts ) {
	syscall( CG_R_ADDPOLYTOSCENE, hShader, numVerts, verts );
}

void	cg_trapR_AddPolysToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts, int num ) {
	syscall( CG_R_ADDPOLYSTOSCENE, hShader, numVerts, verts, num );
}

int		cg_trapR_LightForPoint( vec3_t point, vec3_t ambientLight, vec3_t directedLight, vec3_t lightDir ) {
	return syscall( CG_R_LIGHTFORPOINT, point, ambientLight, directedLight, lightDir );
}

void	cg_trapR_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b ) {
	syscall( CG_R_ADDLIGHTTOSCENE, org, cg_Com_Printf(intensity), cg_Com_Printf(r), cg_Com_Printf(g), cg_Com_Printf(b) );
}

void	cg_trapR_AddAdditiveLightToScene( const vec3_t org, float intensity, float r, float g, float b ) {
	syscall( CG_R_ADDADDITIVELIGHTTOSCENE, org, cg_Com_Printf(intensity), cg_Com_Printf(r), cg_Com_Printf(g), cg_Com_Printf(b) );
}

void	cg_trapR_RenderScene( const refdef_t *fd ) {
	syscall( CG_R_RENDERSCENE, fd );
}

void	cg_trapR_SetColor( const float *rgba ) {
	syscall( CG_R_SETCOLOR, rgba );
}

void	cg_trapR_DrawStretchPic( float x, float y, float w, float h, 
							   float s1, float t1, float s2, float t2, qhandle_t hShader ) {
	syscall( CG_R_DRAWSTRETCHPIC, cg_Com_Printf(x), cg_Com_Printf(y), cg_Com_Printf(w), cg_Com_Printf(h), cg_Com_Printf(s1), cg_Com_Printf(t1), cg_Com_Printf(s2), cg_Com_Printf(t2), hShader );
}

void	cg_trapR_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs ) {
	syscall( CG_R_MODELBOUNDS, model, mins, maxs );
}

int		cg_trapR_LerpTag( orientation_t *tag, clipHandle_t mod, int startFrame, int endFrame, 
					   float frac, const char *tagName ) {
	return syscall( CG_R_LERPTAG, tag, mod, startFrame, endFrame, cg_Com_Printf(frac), tagName );
}

void	cg_trapR_RemapShader( const char *oldShader, const char *newShader, const char *timeOffset ) {
	syscall( CG_R_REMAP_SHADER, oldShader, newShader, timeOffset );
}

void		cg_trapGetGlconfig( glconfig_t *glconfig ) {
	syscall( CG_GETGLCONFIG, glconfig );
}

void		cg_trapGetGameState( gameState_t *gamestate ) {
	syscall( CG_GETGAMESTATE, gamestate );
}

void		cg_trapGetCurrentSnapshotNumber( int *snapshotNumber, int *serverTime ) {
	syscall( CG_GETCURRENTSNAPSHOTNUMBER, snapshotNumber, serverTime );
}

qboolean	cg_trapGetSnapshot( int snapshotNumber, snapshot_t *snapshot ) {
	return syscall( CG_GETSNAPSHOT, snapshotNumber, snapshot );
}

qboolean	cg_trapGetServerCommand( int serverCommandNumber ) {
	return syscall( CG_GETSERVERCOMMAND, serverCommandNumber );
}

int			cg_trapGetCurrentCmdNumber( void ) {
	return syscall( CG_GETCURRENTCMDNUMBER );
}

qboolean	cg_trapGetUserCmd( int cmdNumber, usercmd_t *ucmd ) {
	return syscall( CG_GETUSERCMD, cmdNumber, ucmd );
}

void		cg_trapSetUserCmdValue( int stateValue, float sensitivityScale ) {
	syscall( CG_SETUSERCMDVALUE, stateValue, cg_Com_Printf(sensitivityScale) );
}

void		testPrintInt( char *string, int i ) {
	syscall( CG_TESTPRINTINT, string, i );
}

void		testPrintFloat( char *string, float f ) {
	syscall( CG_TESTPRINTFLOAT, string, cg_Com_Printf(f) );
}

int cg_trapMemoryRemaining( void ) {
	return syscall( CG_MEMORY_REMAINING );
}

qboolean cg_trapKey_IsDown( int keynum ) {
	return syscall( CG_KEY_ISDOWN, keynum );
}

int cg_trapKey_GetCatcher( void ) {
	return syscall( CG_KEY_GETCATCHER );
}

void cg_trapKey_SetCatcher( int catcher ) {
	syscall( CG_KEY_SETCATCHER, catcher );
}

int cg_trapKey_GetKey( const char *binding ) {
	return syscall( CG_KEY_GETKEY, binding );
}

int cg_trapPC_AddGlobalDefine( char *define ) {
	return syscall( CG_PC_ADD_GLOBAL_DEFINE, define );
}

int cg_trapPC_LoadSource( const char *filename ) {
	return syscall( CG_PC_LOAD_SOURCE, filename );
}

int cg_trapPC_FreeSource( int handle ) {
	return syscall( CG_PC_FREE_SOURCE, handle );
}

int cg_trapPC_ReadToken( int handle, pc_token_t *pc_token ) {
	return syscall( CG_PC_READ_TOKEN, handle, pc_token );
}

int cg_trapPC_SourceFileAndLine( int handle, char *filename, int *line ) {
	return syscall( CG_PC_SOURCE_FILE_AND_LINE, handle, filename, line );
}

void	cg_trapS_StopBackgroundTrack( void ) {
	syscall( CG_S_STOPBACKGROUNDTRACK );
}

int cg_trapRealTime(qtime_t *qtime) {
	return syscall( CG_REAL_TIME, qtime );
}

void cg_trapSnapVector( float *v ) {
	syscall( CG_SNAPVECTOR, v );
}

// this returns a handle.  arg0 is the name in the format "idlogo.roq", set arg1 to NULL, alteredstates to qfalse (do not alter gamestate)
int cg_trapCIN_PlayCinematic( const char *arg0, int xpos, int ypos, int width, int height, int bits) {
  return syscall(CG_CIN_PLAYCINEMATIC, arg0, xpos, ypos, width, height, bits);
}
 
// stops playing the cinematic and ends it.  should always return FMV_EOF
// cinematics must be stopped in reverse order of when they are started
e_status cg_trapCIN_StopCinematic(int handle) {
  return syscall(CG_CIN_STOPCINEMATIC, handle);
}


// will run a frame of the cinematic but will not draw it.  Will return FMV_EOF if the end of the cinematic has been reached.
e_status cg_trapCIN_RunCinematic (int handle) {
  return syscall(CG_CIN_RUNCINEMATIC, handle);
}
 

// draws the current frame
void cg_trapCIN_DrawCinematic (int handle) {
  syscall(CG_CIN_DRAWCINEMATIC, handle);
}
 

// allows you to resize the animation dynamically
void cg_trapCIN_SetExtents (int handle, int x, int y, int w, int h) {
  syscall(CG_CIN_SETEXTENTS, handle, x, y, w, h);
}

/*
qboolean cg_traploadCamera( const char *name ) {
	return syscall( CG_LOADCAMERA, name );
}

void cg_trapstartCamera(int time) {
	syscall(CG_STARTCAMERA, time);
}

qboolean cg_trapgetCameraInfo( int time, vec3_t *origin, vec3_t *angles) {
	return syscall( CG_GETCAMERAINFO, time, origin, angles );
}
*/

qboolean cg_trapGetEntityToken( char *buffer, int bufferSize ) {
	return syscall( CG_GET_ENTITY_TOKEN, buffer, bufferSize );
}

qboolean cg_trapR_inPVS( const vec3_t p1, const vec3_t p2 ) {
	return syscall( CG_R_INPVS, p1, p2 );
}
