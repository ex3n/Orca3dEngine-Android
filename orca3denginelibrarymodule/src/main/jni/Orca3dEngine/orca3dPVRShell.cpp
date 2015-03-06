// oadll.cpp : Defines the exported functions for the DLL application.
//

#include <map>
#include <list>
#include <vector>
#include <valarray>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;

#include <string.h>

#include "PVRShell.h"
#include "OGLES2Tools.h"

#include "PVRShellAPI.h"
#include "PVRShellOS.h"
#include "PVRShellImpl.h"

#include "Orca3dAndroid.h"
#include "logger.h"

#define LOG_TAG "Orca3dPVRShell"
PVRShellInit init;

#ifdef GL2JNI // ANDROID
int pvrInit(ANativeWindow *w)
{
    LOG_INFO("pvrInitStart");
	init.Init();
	PVRShell* m_pShell = init.m_pShell;
	m_pShell->InitApplication();
    glEnable(GL_DEPTH_TEST);

	init.m_NWT = w;
    init.OsDoInitAPI();
    LOG_INFO("pvrInitEnd");
	return 0;
}

#endif

int pvrDeinit()
{
	init.Deinit();
	return 0;
}

int pvrInit(EGLDisplay d, EGLContext c, EGLSurface s)
{
	init.Init();
	PVRShell* m_pShell = init.m_pShell;
	m_pShell->InitApplication();
	
	init.m_EGLDisplay = d;
	init.m_EGLContext = c;
	init.m_EGLWindow = s;

	EGLint dim;
	eglQuerySurface(d, s, EGL_WIDTH, &dim);
	m_pShell->PVRShellSet(prefWidth, dim);
	eglQuerySurface(d, s, EGL_HEIGHT, &dim);
	m_pShell->PVRShellSet(prefHeight, dim);


	return 0;
}

PVRShell* NewDemo()
{
    LOG_INFO("return orca3d");
	return new Orca3dAndroid(512, 512);
	LOG_INFO("orca3d returned");
}

void pvrLoadModel(const char* readPath, const char* podFile, const char* fxFile)
{
    LOG_INFO("load model start");
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;

	LOG_INFO("Orca3d new from Orca3dAndroid");
	if (orca3d)
	{
		orca3d->LoadModel(readPath, podFile, fxFile);
		LOG_INFO("modelLoaded");
		//float width = (float)orca3d->PVRShellGet(prefWidth);
		//float height = (float)orca3d->PVRShellGet(prefHeight);
		LOG_INFO("getting controller");
		orca3d->getController()->setViewSize(512,512);//width, height);
		LOG_INFO("found controller");
	}
		
}

bool pvrIsModelLoaded()
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	if (orca3d)
		return orca3d->getController()->isInitialized();

	return false;
}

void pvrUnloadModel()
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->UnloadModel();
}

int pvrRender(float timeDelta)
{

	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	if (orca3d){
//	LOG_INFO("orca3d getController renderFrame start");
    //glClear(GL_DEPTH_BUFFER_BIT);
    //	LOG_INFO("GLClear");
	    if(orca3d->getController())
	    {
	    bool controllerGood = orca3d->getController()->isInitialized();
	    if(controllerGood)
	    	//LOG_INFO("orca3d gotController init is true");
		orca3d->getController()->renderFrame(timeDelta);
//		LOG_INFO("orca3d getController renderFrame end");
		}
//		LOG_INFO("wtf");
		}

	init.ApiRenderComplete();

	return 0;
}

 GLubyte *pvrRenderToImageData(int bufferWidth, int bufferHeight, int dataLength)
 {
    Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
    GLubyte *renderedImage = orca3d->getController()->renderToImageData(bufferWidth, bufferHeight,dataLength);
    return renderedImage;
 }

int pvrRelease()
{
	PVRShell* m_pShell = init.m_pShell;
	if (!m_pShell)
		return 0;

	m_pShell->ReleaseView();

	init.OsDoReleaseAPI();

	m_pShell->QuitApplication();

	return 0;
}

void pvrSetZoom(float zoom)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setZoom(zoom);
}

void pvrSetZoomScale(float scale)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setZoomScale(scale);
}


float pvrGetZoom()
{
    Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
    float zoom = orca3d->getController()->getZoom();
    return zoom;
}

void pvrHomePosition(float animationDuration)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->homePosition(animationDuration);
}

void pvrRotate(float xAxisAngle, float yAxisAngle, float zAxisAngle)
{
	printf("I made it to the rotate\n");
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->rotate(xAxisAngle, yAxisAngle, zAxisAngle);
}

void pvrPan(float x, float y)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->pan(x, y);
}

void pvrSetPanScale(float scale)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setPanScale(scale);
}

void pvrShowLabels(bool show, const char *layerName)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setLabelsVisible(show, layerName);
}

void pvrShowLayers(const char ** layers, int count)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	for (int i = 0; i < count; i++)
	{   LOG_INFO("Layer time %s" , layers[i]);
		orca3d->getController()->setLayerVisible(layers[i], true);
	}
}

void pvrShowLayer(const char * layerName, bool show)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setLayerVisible(layerName, show);
}
void setLayerHighlighted(const char *layerName, bool highlighted)
{
    Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
    orca3d->getController()->setLayerHighlighted(layerName,highlighted);
}


void pvrHideLayers(const char ** layers, int count)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	for (int i = 0; i < count; i++)
	{
		orca3d->getController()->setLayerVisible(layers[i], false);
	}
}

void pvrSize(int width, int height)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setViewSize((float)(width), (float)(height));
	
	init.m_pShell->PVRShellSet(prefWidth, width);
}

void pvrPanScale(float scale)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setPanScale(scale);
}

void pvrLightScale(float scale)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setLightScale(scale);
}

void pvrZoomLimits(float min, float max)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setZoomLimits(min, max);
}

void pvrAddLayer(const char * layerName)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;

	orca3d->getController()->addLayer(layerName);
}

void pvrAddNodeNameSubstringForLayer(const char * layerName, const char * substring)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->addNodeNameSubstringForLayer(layerName, substring);
}

void pvrExcludeNodeNameSubstringForLayer(const char * layerName, const char * substring)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->excludeNodeNameSubstringFromLayer(layerName, substring);

}



void pvrEnableAlphaBlend(bool enable, bool transparentSkin)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	LOG_INFO("set up alpha blend");
	orca3d->getController()->enableAlphaBlend(enable, transparentSkin);
}


void pvrAlphaNodeNameSubstringForAllLayers(const char * substring)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->addAlphaNodeNameSubstringForAllLayers(substring);
}

void pvrAssignLayersAndAlpha()
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->assignLayersAndAlphaToNodes();
}

void pvrRotationOffset(float xAxisAngle, float yAxisAngle, float zAxisAngle)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setRotationOffset(xAxisAngle, yAxisAngle, zAxisAngle);
}

void pvrWorldCenterOffset(float x, float y, float z)
{
    Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
    orca3d->getController()->setWorldCenterOffset(x,y,z);
}


void pvrAddTimeAnimation(const char *name, float duration, bool repeats, int startFrame, int endFrame, bool oscillates)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->addTimeAnimation(name, duration, repeats, startFrame, endFrame, oscillates);
}

void pvrAcitvateAnimation(const char *name, bool activate)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->activateAnimation(name, activate);
}

void pvrAddControl1DAnimation(const char *name, int startFrame, int endFrame, bool oscillates)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->addControl1DAnimation(name, startFrame, endFrame, oscillates);
}

void pvrSetAnimationProgress(const char *name, float progress)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setAnimationProgress(name, progress);
}
void pvrAddControl2DAnimation(const char *  name, const char *  type)
{
    Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
    orca3d->getController()->addControl2DAnimation( name, type);
}
void setAnimation2DProgress(const char * name, float progress1, float progress2)
{
    Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
    orca3d->getController()->setAnimation2DProgress(name,progress1,progress2);
}
void pvrSetLabelScale(float scale)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setLabelScale(scale);
}

void pvrPickLabel(float xPosition, float yPosition)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->pickLabelAtViewPosition(xPosition, yPosition);
}

void pvrPickLabelNamed(const char *name)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	return orca3d->getController()->pickLabelNamed(name);
}

bool pvrGetLabelInfo(char **labelName, float *xPosition, float *yPosition)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	return orca3d->getController()->getPickedLabelInfo(labelName, xPosition, yPosition);
}

void pvrGetLabelsForLayer(const char *layerName, std::vector<std::string>* labelNames)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	return orca3d->getController()->getLabelNamesForLayer(layerName, labelNames);
}

void pvrHighlightLayer(const char *layerName, bool highlight)
{
	Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
	orca3d->getController()->setLayerHighlighted(layerName, highlight);
}

void pvrSetRotationOffsetAxis(float xVal, float yVal, float zVal)
{
    Orca3dAndroid* orca3d = (Orca3dAndroid*)init.m_pShell;
    orca3d->getController()->setRotationOffset(xVal,yVal,zVal);
}



void NSLog(char const * const format, ...)
{
	va_list arg;
	char	buf[1024];

	va_start(arg, format);
	vsnprintf(buf, 1024, format, arg);
	va_end(arg);

#if defined(WIN32)
	OutputDebugStringA(buf);
	OutputDebugStringA("\r\n");
#elif defined(ANDROID)
	__android_log_write(ANDROID_LOG_INFO, "Orca3d", buf);
#endif
}

#define PATHCCH_MAX_CCH             0x8000

bool
PathCchFindExtensionA(
const char *pszPath,
int cchPath,
const char **ppszExt)
{
	const char *pszDot;

	*ppszExt = NULL;

	if (pszPath == NULL || cchPath == 0 || cchPath > PATHCCH_MAX_CCH)
	{
		return false;
	}

	pszDot = NULL;

	while (cchPath && (*pszPath != '\0'))
	{
		switch (*pszPath)
		{
		case '.':
			pszDot = pszPath;   // remember the last dot
			break;

		case '\\':
		case ' ':               // extensions can't have spaces
			pszDot = NULL;      // forget last dot, it was in a directory
			break;
		}

		pszPath = pszPath++;//CharNextA(pszPath);
		cchPath--;
	}

	if (cchPath == 0)
	{
		// the string is longer than cchPath
		return false;
	}

	// if we found the extension, return ptr to the dot, else
	// ptr to end of the string (NULL extension)
	*ppszExt = pszDot ? pszDot : pszPath;

	return true;
}

bool
PathCchRemoveExtensionA(
char* pszPath,
int cchPath)
{
	bool hr;
	char* pszExt;

	// Returns pointer to end of string if no extension
	hr = PathCchFindExtensionA(pszPath, cchPath, (const char**)&pszExt);
	if (hr)
	{
		*pszExt = '\0';
	}
	return hr;
}

bool
PathCchRenameExtensionA(
char* pszPath,
int cchPath,
const char* pszExt)
{
	bool hr;
	char* pszOldExt;

	// Returns pointer to end of string if no extension
	hr = PathCchFindExtensionA(pszPath, cchPath, (const char**)&pszOldExt);
	if (hr)
	{
		strncpy(pszOldExt, pszExt, cchPath - (pszOldExt - pszPath));
	}
	return hr;
}

