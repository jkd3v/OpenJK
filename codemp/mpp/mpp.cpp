
/**************************************************
* MultiPlugin++ by Deathspike
*
* Client-side plugin project which allows any
* developer to use this framework as a basis for
* their own plugin.
**************************************************/

#include <mpp/mppHeader.h>

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

mpp_clientInfo_t	clientInfo[MAX_CLIENTS];
vec3_t				currentOrigin[MAX_GENTITIES];
qboolean			currentValid[MAX_GENTITIES];
refdef_t			refdef;
snapshot_t			snap;

/**************************************************
* LoadPlugins
*
* Builds the list of plugins for this module, does
* not load them up though. This will be done when
* the initialization call is coming in.
**************************************************/

extern vm_t *cgvm;
void LoadPlugins()
{
		int numFiles;
		char ** filesList = Sys_ListFiles("plugins", ".dll", "", &numFiles, qfalse);
		
		for (int i = 2; i < numFiles; i++) { // Ignore "." and ".."
			if (cgvm->isLegacy) {
				if (!Q_stricmpn(filesList[i]+1, "legacy_", 7)) {
					mppPluginLoad(va("plugins/%s", filesList[i]+1));
				}
			}
			else if (!Q_stricmpn(filesList[i]+1, "openjk_", 7)) {
				mppPluginLoad(va("plugins/%s", filesList[i]+1));
			}
		}

		Sys_FreeFileList(filesList);
}

/**************************************************
* UnloadPlugins
*
* Unload the plugins.
**************************************************/

void UnloadPlugins()
{
	MultiModule_t *pMultiModule = MultiModule;
	while (pMultiModule) {
		MultiModule_t *next = (MultiModule_t *)pMultiModule->pNextPlugin;
		SDL_UnloadObject(pMultiModule->hMod);
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
}

void mppDestroy() {
	if (mppIsInitialized == qfalse) return;
	mppIsInitialized = qfalse;
	UnloadPlugins();
}