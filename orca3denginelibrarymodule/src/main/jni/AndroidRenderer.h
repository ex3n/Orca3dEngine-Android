//
// Copyright 2011 Tero Saarni
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <pthread.h>
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#include <time.h>

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))
#endif

// OLD
int pvrInit(const char* podFile, const char* fxFile);
int pvrDeinit();

// NEW
#define VIEW_MODE_STANDARD "Standard"
#define VIEW_MODE_SIDE_SLICE "Side Slice"
#define VIEW_MODE_TOP_SLICE "Top Slice"
#define LAYER_FAT "Fat"
#define LAYER_FAT_LABELS "Fat Labels"
#define LAYER_OUTER_HEART "Outer Heart"
#define LAYER_OUTER_HEART_LABELS "Outer Heart Labels"
#define LAYER_BLOOD_VESSELS "Blood Vessels"
#define LAYER_BLOOD_VESSELS_LABELS "Blood Vessels Labels"
#define LAYER_ALWAYS_ON "Always On"

int  pvrInit(ANativeWindow *w);
void pvrLoadModel(const char* readPath, const char* podFile, const char* fxFile);
bool pvrIsModelLoaded();
void pvrUnloadModel();
int  pvrRender(float timeDelta);
void pvrSetAnimation(bool state);
void pvrShowLabels(bool show, const char *layerName);
void pvrShowLayer(const char *layerName, bool show);
void pvrShowLayers(const char ** layers, int count);
void pvrHideLayers(const char ** layers, int count);
void pvrRotate(float xAxisAngle, float yAxisAngle, float zAxisAngle);
void pvrPan(float x, float y);
void pvrSetZoom(float zoom);
void pvrSetZoomScale(float scale);
void pvrSize(int width, int height);
void pvrHomePosition(float animationDuration);
void pvrPanScale(float scale);
void pvrLightScale(float scale);
void pvrZoomLimits(float min, float max);
void pvrAddLayer(const char * layerName);
void pvrNodeNameSubstringForLayer(const char * layerName, const char * substring, bool exclude);
void pvrAddNodeNameSubstringForLayer(const char * layerName, const char * substring);
void pvrEnableAlphaBlend(bool enable, bool transparentSkin);
void pvrAlphaNodeNameSubstringForAllLayers(const char * substring);
void pvrAssignLayersAndAlpha();
void pvrRotationOffset(float xAxisAngle, float yAxisAngle, float zAxisAngle);
void pvrSetPanScale(float scale);
void pvrAddTimeAnimation(const char *name, float duration, bool repeats, int startFrame, int endFrame, bool oscillates);
void pvrAcitvateAnimation(const char *name, bool activate);
void pvrSetAnimationProgress(const char *name, float progress);
void pvrSetLabelScale(float scale);
void pvrPickLabel(float xPosition, float yPosition);
void pvrPickLabelNamed(const char *name);
bool pvrGetLabelInfo(char **labelName, float *xPosition, float *yPosition);
void pvrExcludeNodeNameSubstringForLayer(const char * layerName, const char * substring);
void pvrWorldCenterOffset(float x,float y,float z);
void pvrAddControl1DAnimation(const char *name, int startFrame, int endFrame, bool oscillates);


// void pvrGetLabelsForLayer(const char *layerName, std::vector<std::string>* labelNames);

class Renderer {
public:
    Renderer(const char* path, const char* podFile, const char* fxFile);
    virtual ~Renderer();

    // Following methods can be called from any thread.
    // They send message to render thread which executes required actions.
    void start();
    void stop();
    void setWindow(ANativeWindow* window);
    void setInitilizeMessage();
    bool doneInit();
    
private:
    enum RenderThreadMessage {
        MSG_NONE = 0,
        MSG_WINDOW_SET,
        MSG_RENDER_LOOP_EXIT
    };

    enum ViewMode
    {
    	Standard,
    	SideSlice,
    	TopSlice
    };

    clock_t _tick;
    pthread_t _threadId;
    pthread_mutex_t _mutex;
    enum RenderThreadMessage _msg;
    
    // android window, supported by NDK r5 and newer
    ANativeWindow* _window;

    bool _initialized;
    bool _resourceReady;
	bool m_bAnimationEnabled;
	ViewMode m_viewMode;
    
    // RenderLoop is called in a rendering thread started in start() method
    // It creates rendering context and renders scene until stop() is called
    void renderLoop();
    
    bool initialize();
    void destroy();

    void createResources();
    void drawFrame();


    // Helper method for starting the thread 
    static void* threadStartCallback(void *myself);
};

#endif // RENDERER_H
