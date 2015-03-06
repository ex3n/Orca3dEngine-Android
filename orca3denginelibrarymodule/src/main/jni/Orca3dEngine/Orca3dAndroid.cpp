#include "Orca3dAndroid.h"
//#include "nslog.h"

#include "PVRShellAPI.h"
#include "PVRShellOS.h"
#include "PVRShellImpl.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "logger.h"

#define LOG_TAG "orca3dAndroid"

//int ii = 0;

Orca3dAndroid::Orca3dAndroid(int width, int height)
{
	m_width = width;
	m_height = height;
	m_pController = NULL;
}

Orca3dAndroid::~Orca3dAndroid()
{
	UnloadModel();
}

Orca3dController* Orca3dAndroid::getController()
{
	return m_pController;
}

void Orca3dAndroid::LoadModel(const char *readPath, const char *podFile, const char *fxFile)
{
    LOG_INFO("loading Model");
	UnloadModel();
	m_pController = new Orca3dController();
	LOG_INFO("Created controller");
	LOG_INFO("Load the %s" , podFile);
	m_pController->setModelFiles(readPath, podFile, fxFile);
}

void Orca3dAndroid::UnloadModel()
{
	if (m_pController)
	{
		delete m_pController;
		m_pController = NULL;
	}
}

bool Orca3dAndroid::InitApplication()
{
    LOG_INFO("InitApplication");
	//CPVRTResourceFile::SetReadPath(m_readPath.c_str());

	// Get and set the load/release functions for loading external files.
	// In the majority of cases the PVRShell will return NULL function pointers implying that
	// nothing special is required to load external files.
	CPVRTResourceFile::SetLoadReleaseFunctions(PVRShellGet(prefLoadFileFunc), PVRShellGet(prefReleaseFileFunc));

	PVRShellSet(prefFullScreen, false);
	PVRShellSet(prefDepthBPP,24);


	return true;
}

bool Orca3dAndroid::QuitApplication()
{
	return true;
}

bool Orca3dAndroid::InitView()
{
    LOG_INFO("win Init View");
	m_pController->setViewSize((float)(m_width), (float)(m_height));

    PVRShellSet(prefDepthBPP,16);
    PVRShellSet(prefFSAAMode,4);

	return true;
}

bool Orca3dAndroid::ReleaseView()
{
	//_orientationChangedObserver = nil;

	return true;
}

bool Orca3dAndroid::RenderScene()
{
    LOG_INFO("renderScene");
   glClear(GL_DEPTH_BUFFER_BIT);



    //  glDepthMask(true);
      //LOG_INFO("glDepth mask set to true");
	return true;
}


