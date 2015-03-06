package com.orca.orca3denginelibrarymodule;/*
 * Copyright (C) 2007 The Android Open Source Project
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

import android.content.res.AssetManager;
import android.view.Surface;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

// Wrapper for native library

public class Orca3DEngineLib {

     static {
         System.loadLibrary("Orca3DEngine");
     }

     public static native void createAssetManager(AssetManager assetManager);
     
     public static native void nativeOnStart(String path, String podFile, String fxFile);
     public static native void nativeOnResume();
     public static native void nativeOnPause();
     public static native void nativeOnStop();
     public static native void nativeSetSurface(Surface surface);
     public static native void setTouchRotate(float x, float y);
     public static native void setTouchZoom(float z);
     public static native void setAnimation(boolean state);
     public static native void setLabelsVisible(String layerName, boolean show);
     public static native void setLayer(int layer);
     public static native void setLayerVisible(String layerName, boolean show);
     public static native void excludeNodeNameSubstringForLayer(String LayerName, String excludedsubString );
     public static native void setTouchReset();
     public static native void setTouchPan(float x, float y);
     public static native void addLayer(String layerName);
     public static native void addNodeNameSubstringForLayer(String layerName,String subString);
     public static native void loadModel(String readPath, String podFile, String fxFile);
     public static native void resourcesLoaded();
     public static native void setLabelScale(float scaleValue);
     public static native void enableAlphaBlend(boolean enable, boolean transparentSkin);
     public static native void addAlphaNodeNameSubstringForAllLayers(String substring);
     public static native void assignLayersAndAlphaToNodes();
     public static native boolean doneInit();
     public static native void setZoomScale(float zoomScale);
     public static native void setZoomMin(float zoomMin, float zoomMax);
     public static native void setRotationOffsetAxis(float x,float y,float z);
     public static native void setWorldCenterOffset(float x,float y, float z);
    public static native void setPanScale(float scale);
    public static native void setLightScale(float scale);
    public static native void addTimeAnimation(String name, float duration, boolean repeats, int startFrame, int endFrame, boolean oscillates);
    public static native void activateAnimations(String name, boolean active);
    public static native void addControl1DAnimations(String name,int startFrame, int endFrame, boolean oscillates);
    public static native void setSize(float width, float height);
}
