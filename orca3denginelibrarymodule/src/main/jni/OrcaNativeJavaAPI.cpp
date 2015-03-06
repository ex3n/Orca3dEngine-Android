/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// OpenGL ES 2.0 code

#include <jni.h>
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "logger.h"
#include "AndroidRenderer.h"


ANativeWindow *g_window = 0;
static Renderer *renderer = 0;

extern "C" {
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_createAssetManager( JNIEnv* env, jobject obj, jobject assetManager );
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_nativeOnStart(JNIEnv* jenv, jobject obj,jstring jpath, jstring podFile, jstring jfxFile);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_nativeOnResume(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_nativeOnPause(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_nativeOnStop(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setTouchRotate(JNIEnv* env, jobject obj, jfloat jx, jfloat jy);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setTouchZoom(JNIEnv* env, jobject obj, jfloat jz);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setAnimation(JNIEnv* env, jobject obj, jboolean jstate);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setLayer(JNIEnv* env, jobject obj, jint jlayer);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setLayerVisible(JNIEnv* env, jobject obj, jstring jlayerName, jboolean show);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setTouchReset(JNIEnv* env, jobject obj);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setTouchPan(JNIEnv* env, jobject obj,jfloat jx, jfloat jy);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_addLayer(JNIEnv* env, jobject obj, jstring jlayerName);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_addNodeNameSubstringForLayer(JNIEnv* env, jobject obj, jstring jlayerName,jstring jsubString);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_loadModel(JNIEnv* env, jobject obj, jstring jreadPath, jstring jpodFile, jstring jfxFile);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_resourcesLoaded(JNIEnv* env,jobject obj);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setLabelScale(JNIEnv* env, jobject obj, jfloat jscaleValue);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_enableAlphaBlend(JNIEnv* env, jobject obj, jboolean jenable, jboolean transparentSkin);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_addAlphaNodeNameSubstringForAllLayers(JNIEnv* env, jobject obj, jstring jsubstring);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_assignLayersAndAlphaToNodes(JNIEnv* env, jobject obj);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setLabelsVisible(JNIEnv* env, jobject obj, jstring jlayerName, jboolean show);
    JNIEXPORT bool JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_doneInit(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setZoomScale(JNIEnv* jenv, jobject obj,jfloat zoomScale);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setZoomMin(JNIEnv* jenv, jobject obj,jfloat zoomMin,jfloat zoomMax);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setRotationOffsetAxis(JNIEnv* jenv, jobject obj,jfloat x,jfloat y, jfloat z);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setWorldCenterOffset(JNIEnv* jenv, jobject obj,jfloat x,jfloat y, jfloat z);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setPanScale(JNIEnv* jenv, jobject obj,jfloat scale);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setLightScale(JNIEnv* jenv, jobject obj,jfloat scale);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_addTimeAnimation(JNIEnv* jenv, jobject obj,jstring name, jfloat duration, jboolean repeats, jint startFrame, jint endFrame, jboolean oscillates);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_activateAnimations(JNIEnv* jenv, jobject obj, jstring jname, jboolean active);     JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_addControl1DAnimations(JNIEnv* jenv, jobject obj, jstring jname,jint startFrame, jint endFrame, jboolean oscillates);
    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setSize(JNIEnv* jenv, jobject obj,jfloat width, jfloat height);


};

#define  LOG_TAG    "OrcaNativeJavaAPI"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

extern AAssetManager* g_AssetManager;
JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_createAssetManager(JNIEnv* env, jobject obj, jobject assetManager)
{
	LOGI("native createAssetManager\n");

    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    assert( mgr );

    // Store the assest manager for future use.
    // Declared in PVRShellOS.cpp
    g_AssetManager = mgr;
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_nativeOnStart(JNIEnv* jenv, jobject obj,jstring jpath, jstring jpodFile, jstring jfxFile)
{
    LOG_INFO("nativeOnStart");
    const char *path = jenv->GetStringUTFChars(jpath,0);

    const char *podFile = jenv->GetStringUTFChars(jpodFile, 0);
            LOGI("Pod File %s", podFile);
            const char *fxFile = jenv->GetStringUTFChars(jfxFile, 0);
            LOGI("fxfile %s", fxFile);

    renderer = new Renderer(path, podFile,fxFile);

    return;
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_nativeOnResume(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnResume");
    renderer->start();

    return;
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_nativeOnPause(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnPause");
    renderer->stop();
    return;
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_nativeOnStop(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnStop");
    delete renderer;
    renderer = 0;
    LOG_INFO("Releasing window");
            ANativeWindow_release(g_window);
    return;
}

JNIEXPORT bool JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_doneInit(JNIEnv* jenv, jobject obj)
{
    return renderer->doneInit();
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface)
{
    if (surface != 0) {
        g_window = ANativeWindow_fromSurface(jenv, surface);
        LOG_INFO("Got window %p", g_window);
        renderer->setWindow(g_window);
        LOG_INFO("window was set");
    } else {
        LOG_INFO("Releasing window");
        ANativeWindow_release(g_window);
    }

    return;
}

// ==========================================
// User interface helpers

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setTouchRotate(JNIEnv* env, jobject obj, jfloat jx, jfloat jy)
{
	float x = jx;
	float y = jy;
	 //LOGI("native setTouchRotate(%f, %f)\n", x, y);

	// TODO:
	pvrRotate(y, x, 0.0);
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setTouchZoom(JNIEnv* env, jobject obj, jfloat jz)
{
	float z = jz;
	// LOGI("native setTouchZoom(%f)\n", z);

	pvrSetZoom(z);
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setTouchReset(JNIEnv* env, jobject obj)
{
	pvrHomePosition(.3);
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setTouchPan(JNIEnv* env, jobject obj,jfloat jx, jfloat jy)
{
	pvrPan(jx,jy);
}


JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setAnimation(JNIEnv* env, jobject obj, jboolean jstate)
{
	bool state = jstate;

	// TODO:
	pvrAcitvateAnimation(VIEW_MODE_STANDARD, state);
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setLayer(JNIEnv* env, jobject obj, jint jlayer)
{
	int layer = jlayer;
	const char* layers[] = { LAYER_OUTER_HEART, LAYER_ALWAYS_ON };

	LOGI("native setLayer(%d)\n", layer);

    if (layer > ARRAYSIZE(layers)) 
    {
        layer = ARRAYSIZE(layers);
    }
    if (layer <= 0) 
    {
        layer = 1;
    }

    pvrHideLayers(layers,layer);
    pvrShowLayer(LAYER_OUTER_HEART, true);
    pvrShowLayer(LAYER_ALWAYS_ON,true);


}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_addLayer(JNIEnv* env, jobject obj, jstring jlayerName)
{
    //LOGI("adding the layer");
    const char *layerName = env->GetStringUTFChars(jlayerName, 0);
    //LOGI("Converted to Char*");

    pvrAddLayer(layerName);
    LOGI("Layer added %s", layerName);
}


JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_addNodeNameSubstringForLayer(JNIEnv* env, jobject obj, jstring jlayerName,jstring jsubString)
{
    const char *nativeLayerName= env->GetStringUTFChars(jlayerName, 0);
    const char *nativeSubStringName= env->GetStringUTFChars(jsubString, 0);
    pvrAddNodeNameSubstringForLayer(nativeLayerName,nativeSubStringName);
}

    JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_loadModel(JNIEnv* env, jobject obj, jstring jreadPath, jstring jpodFile, jstring jfxFile)
    {
        const char *readPath = env->GetStringUTFChars(jreadPath, 0);

        LOGI("Read Path %s", readPath);
        const char *podFile = env->GetStringUTFChars(jpodFile, 0);
        LOGI("Pod File %s", podFile);
        const char *fxFile = env->GetStringUTFChars(jfxFile, 0);
        LOGI("fxfile %s", fxFile);
        pvrLoadModel(readPath, podFile, fxFile);
    }

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_resourcesLoaded(JNIEnv* env,jobject obj)
    {
        renderer->setInitilizeMessage();
    }

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setLayerVisible(JNIEnv* env, jobject obj, jstring jlayerName, jboolean show)
    {
        const char *layerName = env->GetStringUTFChars(jlayerName, 0);

        pvrShowLayer(layerName, (bool)show);
    }

 JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_excludeNodeNameSubstringForLayer(JNIEnv* env, jobject obj, jstring jlayerName,jstring jexsubString)
    {
        const char *layerName = env->GetStringUTFChars(jlayerName, 0);
        const char *excludedSub = env->GetStringUTFChars(jexsubString, 0);
        pvrExcludeNodeNameSubstringForLayer(layerName,excludedSub);

    }
JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setLabelsVisible(JNIEnv* env, jobject obj, jstring jlayerName, jboolean show)
    {
        const char *layerName = env->GetStringUTFChars(jlayerName,0);
        pvrShowLabels((bool)show,layerName);

    }
JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setLabelScale(JNIEnv* env, jobject obj, jfloat jscaleValue)
        {
        pvrSetLabelScale(jscaleValue);

        }
JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_enableAlphaBlend(JNIEnv* env, jobject obj, jboolean jenable, jboolean transparentSkin)
        {LOG_INFO("setup alpha blend");
        pvrEnableAlphaBlend((bool)jenable,(bool) transparentSkin);
        }
JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_addAlphaNodeNameSubstringForAllLayers(JNIEnv* env, jobject obj, jstring jsubString)
        {
    const char *subString = env->GetStringUTFChars(jsubString, 0);
        pvrAlphaNodeNameSubstringForAllLayers(subString);
        }
JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_assignLayersAndAlphaToNodes(JNIEnv* env, jobject obj)
        {
        pvrAssignLayersAndAlpha();
        }

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setZoomScale(JNIEnv* jenv, jobject obj,jfloat zoomScale)
{
    pvrSetZoom((float)zoomScale);
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setZoomMin(JNIEnv* jenv, jobject obj,jfloat zoomMin,jfloat zoomMax)
{
    pvrZoomLimits(zoomMin,zoomMax);
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setRotationOffsetAxis(JNIEnv* jenv, jobject obj,jfloat x,jfloat y, jfloat z)
{
    pvrRotationOffset(x,y,z);
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setWorldCenterOffset(JNIEnv* jenv, jobject obj,jfloat x,jfloat y, jfloat z)
{
    pvrWorldCenterOffset(x,y,z);
}
JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setPanScale(JNIEnv* jenv, jobject obj,jfloat scale)
{
    pvrSetPanScale(scale);
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setLightScale(JNIEnv* jenv, jobject obj,jfloat scale)
{
    pvrLightScale(scale);
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_addTimeAnimation(JNIEnv* jenv, jobject obj,jstring name, jfloat duration, jboolean repeats, jint startFrame, jint endFrame, jboolean oscillates)
{
    const char *nameString = jenv->GetStringUTFChars(name, 0);
    pvrAddTimeAnimation(nameString,duration,repeats,startFrame,endFrame,oscillates);
}

JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_activateAnimations(JNIEnv* jenv, jobject obj, jstring jname, jboolean active)
{   const char *name = jenv->GetStringUTFChars(jname,0);

    pvrAcitvateAnimation(name, active);
}
JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_addControl1DAnimations(JNIEnv* jenv, jobject obj, jstring jname,jint startFrame, jint endFrame, jboolean oscillates)
{   const char *name = jenv->GetStringUTFChars(jname,0);
    pvrAddControl1DAnimation(name,startFrame,endFrame,oscillates );
}
JNIEXPORT void JNICALL Java_com_orca_orca3denginelibrarymodule_Orca3DEngineLib_setSize(JNIEnv* jenv, jobject obj,jfloat width, jfloat height)
{
    pvrSize(width,height);
}
