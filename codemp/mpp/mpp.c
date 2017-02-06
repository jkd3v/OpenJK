
/**************************************************
* MultiPlugin++ by Deathspike
*
* Client-side plugin project which allows any
* developer to use this framework as a basis for
* their own plugin.
**************************************************/

#include "mppHeader.h"

/**************************************************
* This is the Shared Buffer across all the modules,
* and contains about any function you'd might wish.
**************************************************/

MultiPlugin_t	 MultiPlugin;
MultiSystem_t	 MultiSystem;
MultiModule_t	*MultiModule;
MultiRender_t	 pRenders[1024];
int				 iRenders = 0;

/**************************************************
* These are data segments filled in by MultiPlugin++ 
* to avoid having to rely on unreliable data addresses,
* which modifications are able to alter.
**************************************************/

clientInfo_t	clientInfo[MAX_CLIENTS];
vec3_t			currentOrigin[MAX_GENTITIES];
qboolean		currentValid[MAX_GENTITIES];
refdef_t		refdef;
snapshot_t		snap;

/**************************************************
* LoadPlugins
*
* Builds the list of plugins for this module, does
* not load them up though. This will be done when
* the initialization call is coming in.
**************************************************/

static void LoadPlugins()
{
	char folderName[256];
	if (GetModuleFileName(NULL, folderName, 256) != 0) {
		{
			char * folderPtr = strrchr(folderName, '\\');
			if (folderPtr) *folderPtr = '\0';
		}
		HANDLE hFind;
		WIN32_FIND_DATA FindData;
		char filesNames[256];
		Com_sprintf(filesNames, 256, "%s\\plugins\\*.dll", folderName);
		hFind = FindFirstFile(filesNames, &FindData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				mppPluginLoad(va("%s\\plugins\\%s", folderName, FindData.cFileName));
			} while (FindNextFile(hFind, &FindData));
			FindClose(hFind);
		}
	}
}

/**************************************************
* UnloadPlugins
*
* Unload the plugins.
**************************************************/

static void UnloadPlugins()
{
	MultiModule_t *pMultiModule = MultiModule;
	while (pMultiModule) {
		MultiModule_t *next = (MultiModule_t *)pMultiModule->pNextPlugin;
		FreeLibrary((HMODULE)pMultiModule->hMod);
		free(pMultiModule);
		pMultiModule = next;
	}
	MultiModule = NULL;
}

static qboolean mppIsInitialized = qfalse;
void mppInit() {
	if (mppIsInitialized == qtrue) return;
	mppIsInitialized = qtrue;
	mppPluginInit(&MultiPlugin);
	mppSystemInit(&MultiSystem);
	MultiPlugin.System = &MultiSystem;
	MultiModule = NULL;
	LoadPlugins();
}

void mppDestroy() {
	if (mppIsInitialized == qfalse) return;
	mppIsInitialized = qfalse;
	UnloadPlugins();
}