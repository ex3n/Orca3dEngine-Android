#include "PVRShell.h"
#include "OGLES2Tools.h"
#include "Orca3dController.h"

class Orca3dAndroid : public PVRShell
{
public:
	Orca3dAndroid(int width, int height);
	~Orca3dAndroid();

	// PVRShell
	virtual bool InitApplication();
	virtual bool InitView();
	virtual bool ReleaseView();
	virtual bool QuitApplication();
	virtual bool RenderScene();

	virtual void LoadModel(const char *readPath, const char *podFile, const char *fxFile);
	virtual void UnloadModel();

	virtual Orca3dController* getController();

private:
	int m_width;
	int m_height;

	Orca3dController *m_pController;
	
};