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

#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>

#include "logger.h"
#include "AndroidRenderer.h"

#define LOG_TAG "Androidrenderer"

const char* c_szStandard = "Standard";
const char* c_szStandardNoAnimation = "Standard no animation";
const char* c_szTopSlice = "Top Slice";
const char* c_szTopSliceOpening = "Top Slice opening";
const char* c_szTopSliceNoAnimation = "Top Slice no animation";
const char* c_szSideSlice = "Side Slice";
const char* c_szSideSliceOpening = "Side Slice opening";
const char* c_szSideSliceNoAnimation = "Side Slice no animation";

const char* mPodFile;
const char* mFxFile;
const char* mPath;

Renderer::Renderer(const char* path, const char* podFile,const char* fxFile)
    : _msg(MSG_NONE), _initialized(false), _resourceReady(false)
{
    LOG_INFO("Renderer instance created");

    mPodFile = podFile;
    mFxFile = fxFile;
    mPath = path;

    pthread_mutex_init(&_mutex, 0);    
    return;
}

Renderer::~Renderer()
{
    //LOG_INFO("Renderer instance destroyed");
    pthread_mutex_destroy(&_mutex);
    return;
}

void Renderer::start()
{
    LOG_INFO("Creating renderer thread");
    pthread_create(&_threadId, 0, threadStartCallback, this);
    return;
}

void Renderer::stop()
{
    LOG_INFO("Stopping renderer thread");

    // send message to render thread to stop rendering
    pthread_mutex_lock(&_mutex);
    _msg = MSG_RENDER_LOOP_EXIT;
    pthread_mutex_unlock(&_mutex);

    pthread_join(_threadId, 0);
    LOG_INFO("Renderer thread stopped");

    return;
}

void Renderer::setWindow(ANativeWindow *window)
{
LOG_INFO("stepped into setwindow");
    // notify render thread that window has changed
    pthread_mutex_lock(&_mutex);
    _msg = MSG_WINDOW_SET;
    _window = window;
    pthread_mutex_unlock(&_mutex);

    LOG_INFO("done with set window");

    return;
}

void Renderer::renderLoop()
{
    //LOG_INFO("renderLoop()");
    bool renderingEnabled = true;

    while (renderingEnabled) {
        pthread_mutex_lock(&_mutex);
        //LOG_INFO("renderLoopmessage %d",_msg);
        // process incoming messages
        switch (_msg) {
            case MSG_WINDOW_SET:
            LOG_INFO("ready to initialize from window set");
                initialize();
                //LOG_INFO("initialize finished");
                break;

            case MSG_RENDER_LOOP_EXIT:
                renderingEnabled = false;
                destroy();
                break;

            default:
                break;
        }
        _msg = MSG_NONE;

        if (_resourceReady) {
           //LOG_INFO("start drawframe");
            drawFrame();
            //LOG_INFO("finish draw frame");
        }

        pthread_mutex_unlock(&_mutex);
    }

    //LOG_INFO("Render loop exits");
    return;
}

bool Renderer::initialize()
{
    LOG_INFO("pvr Init start");
    pvrInit(_window);
    //LOG_INFO("pvr Init done ModelShould be loaded here");

    pvrLoadModel(mPath, mPodFile, mFxFile);
    //LOG_INFO("pvrLoadModel finished");
    _tick = 0;
    _initialized = true;
    glClearColor(1., 1., 1., 0.);
   // createResources();

    //const char* layers[2] = {LAYER_ALWAYS_ON,LAYER_OUTER_HEART};
	//pvrShowLayers(layers, ARRAYSIZE(layers));

    return true;
}

void Renderer::destroy() {
    //LOG_INFO("Destroying context");

    // TODO: call PVR to clean up
    _resourceReady = false;
    _initialized = false;
    pvrDeinit();

    return;
}

void Renderer::drawFrame()
{
	if (_resourceReady)
	{
		float dt = 0;
		if (_tick == 0)
		{
			_tick = clock();
		}
		else
		{
			clock_t nt = clock();
			dt = ((float)(nt - _tick))/(CLOCKS_PER_SEC);
			//LOG_INFO("what is dt it is %f",dt);
			if(dt < 0.01667)
			dt = 0.016667;

			_tick = nt;

		}

		pvrRender(dt);
		////LOG_INFO("pvrRender finish");
	}
}

void* Renderer::threadStartCallback(void *myself)
{
    Renderer *renderer = (Renderer*)myself;

    renderer->renderLoop();
    pthread_exit(0);

    return 0;
}

bool Renderer::doneInit()
{
//    if(_initialized)
//    LOG_INFO(" init is ready");
//
//    if(!_initialized)
//    LOG_INFO("init is not ready");

    return _initialized;

}

void Renderer::createResources()
{
//LOG_INFO("createResources");
	//glClearColor(0., 0., 0., 0.);

	//init layers
//	pvrAddLayer(LAYER_FAT);
//	pvrNodeNameSubstringForLayer(LAYER_FAT, "heartBottomRear_fat_geo", false);
//	pvrNodeNameSubstringForLayer(LAYER_FAT, "heartBottomFront_fat_geo", false);
//
//	pvrAddLayer(LAYER_FAT_LABELS);
//	pvrNodeNameSubstringForLayer(LAYER_FAT_LABELS, "annotation_fat", false);
//
//	pvrAddLayer(LAYER_OUTER_HEART);
//	pvrNodeNameSubstringForLayer(LAYER_OUTER_HEART, "heartBottomRear_muscle_geo", false);
//	pvrNodeNameSubstringForLayer(LAYER_OUTER_HEART, "heartBottomFront_muscle_geo", false);
//
//	pvrAddLayer(LAYER_OUTER_HEART_LABELS);
//	pvrNodeNameSubstringForLayer(LAYER_OUTER_HEART_LABELS, "annotation_muscle", false);
//
//	pvrAddLayer(LAYER_BLOOD_VESSELS);
//	pvrNodeNameSubstringForLayer(LAYER_BLOOD_VESSELS, "heartBottomRear_veins_geo", false);
//	pvrNodeNameSubstringForLayer(LAYER_BLOOD_VESSELS, "heartBottomFront_veins_geo", false);
//
//	pvrAddLayer(LAYER_BLOOD_VESSELS_LABELS);
//	pvrNodeNameSubstringForLayer(LAYER_BLOOD_VESSELS_LABELS, "annotation_fat_veins", false);
//
//	pvrAddLayer(VIEW_MODE_SIDE_SLICE);
//	pvrNodeNameSubstringForLayer(VIEW_MODE_SIDE_SLICE, "annotation_front", false);
//
//	pvrAddLayer(VIEW_MODE_TOP_SLICE);
//	pvrNodeNameSubstringForLayer(VIEW_MODE_TOP_SLICE, "annotation_top", false);
//
//	pvrAddLayer(LAYER_ALWAYS_ON);
//	pvrNodeNameSubstringForLayer(LAYER_ALWAYS_ON, "Valve_geo", false);
//	pvrNodeNameSubstringForLayer(LAYER_ALWAYS_ON, "heartTopRear_geo", false);
//	pvrNodeNameSubstringForLayer(LAYER_ALWAYS_ON, "heartTopFront_geo", false);

//	pvrAssignLayersAndAlpha();
//
//
//	const char* hideLayers[] = {LAYER_ALWAYS_ON,LAYER_OUTER_HEART,LAYER_BLOOD_VESSELS,LAYER_FAT,VIEW_MODE_SIDE_SLICE,VIEW_MODE_TOP_SLICE};
//
//	pvrHideLayers(hideLayers,ARRAYSIZE(hideLayers));
//
//	const char* layers[] = { LAYER_OUTER_HEART, LAYER_ALWAYS_ON };
//	pvrShowLayers(layers, ARRAYSIZE(layers));

	//pvrShowLayer(LAYER_ALWAYS_ON, true);

	pvrLightScale(0.8f);
	pvrZoomLimits(0.5f, 4.0f);
	pvrRotationOffset(0.f, 3.14159265f, 0.f);
	pvrSetPanScale(30.0f);
//	pvrShowLabels(false);
	pvrSetLabelScale(12.0f);

	m_bAnimationEnabled = true;
	m_viewMode = Standard;
	pvrAddTimeAnimation(VIEW_MODE_STANDARD, 1.0f, true, 0, 18, false);
	pvrAcitvateAnimation(VIEW_MODE_STANDARD, true);
	pvrAddTimeAnimation(c_szStandardNoAnimation, 1.0f, true, 0, 0, false);

	//?? should be 151 for the end frame, but some rounding error causes that to crash
	pvrAddTimeAnimation(c_szSideSliceOpening, 3.0f, false, 86, 151, false);
	pvrAddTimeAnimation(VIEW_MODE_SIDE_SLICE, 1.0f, true, 133, 151, false);
	pvrAddTimeAnimation(c_szSideSliceNoAnimation, 1.0, true, 133, 133, false);

	pvrAddTimeAnimation(c_szTopSliceOpening, 3.0f, false, 18, 83, false);
	pvrAddTimeAnimation(VIEW_MODE_TOP_SLICE, 1.0f, true, 67, 85, false);
	pvrAddTimeAnimation(c_szTopSliceNoAnimation, 1.0f, true, 67, 67, false);
}

void Renderer::setInitilizeMessage()
{
    //LOG_INFO("set the resources to ready");
    _resourceReady = true;
}

