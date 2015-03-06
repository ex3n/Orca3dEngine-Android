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


import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.GestureDetector;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;
import android.webkit.WebView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.view.ScaleGestureDetector;


import com.dd.plist.NSArray;
import com.dd.plist.NSDictionary;
import com.dd.plist.NSNumber;
import com.dd.plist.NSObject;
import com.dd.plist.NSString;
import com.dd.plist.PropertyListParser;

import android.view.GestureDetector.OnGestureListener;
import com.orca.orca3denginelibrarymodule.R;


import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

public class Orca3DEngineActivity extends ActionBarActivity implements SurfaceHolder.Callback, OnGestureListener,GestureDetector.OnDoubleTapListener
{
	public static String TAG="gl2jniactivity1";
    public static String DEBUG_TAG = "Gesture : ";
    private GestureDetector gestureWatch;
    static AssetManager sAssetManager;
    // SurfaceView mView;
    Orca3DEngineView _view;
    LinearLayout _contentLayout;
    WebView _wv;
    private static final int DURATION = 300;
    SeekBar _seekBar;
	float _x0 = 0;
	float _y0 = 0;
	float _zoom = 1.0f;
	double _dist = 1.0f;
    public Menu mMenu;
    NSDictionary m_rootDict;
    List<String> m_hideLayerList = new ArrayList<String>();
    public List<String> m_animationList = new ArrayList<String>();
    private ScaleGestureDetector SGD;
    float zoomScale = 1;
    boolean zoomMode = false;
    float fingerDistance = 0;
    boolean twoFingerStart = false;
    String m_Path;
    String m_View;
    String m_Model;
    boolean m_WindowFocus;


    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        
        // Pass the asset manager to the native code
        sAssetManager = getAssets();
        Orca3DEngineLib.createAssetManager(sAssetManager);
        m_WindowFocus = true;


        setContentView(R.layout.gl_view);
        _view = (Orca3DEngineView) findViewById(R.id.glview);

        gestureWatch = new GestureDetector(this,this);
        SGD = new ScaleGestureDetector(this, new ScaleListener());
        // GLSurfaceView glv = (GLSurfaceView)_view;
         //_view.setEGLConfigChooser(8,  8,  8, 8, 16, 8);
        
        Log.i(TAG, "onCreate .................");


        _view.getHolder().addCallback(this);
        String titleName = getIntent().getExtras().getString("Model Name");
        setTitle(titleName);
        m_View = getIntent().getExtras().getString("View Name");
        m_Model = titleName;
        m_Path = getIntent().getExtras().getString("Path");


        
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {



          //String viewLayerName = getIntent().getExtras().getString("View Name");
//        pathAndFile = pathAndFile + "/AnatomyInfo.plist";
//        InputStream is = null;
        try {
//            is = getAssets().open(pathAndFile);
//            NSDictionary rootDict = (NSDictionary) PropertyListParser.parse(is);
            NSObject[] views = ((NSArray) m_rootDict.objectForKey("Views")).getArray();
            for (NSObject dict : views) {
               NSDictionary viewDict = (NSDictionary)dict;

                NSString layerName = (NSString)viewDict.objectForKey("Name");
                String layerNameString = layerName.toString();
                if(layerNameString.equals(m_View)) {
//                    SubMenu submenu = menu.addSubMenu(layerName.toString());
                    NSObject[] viewLayers = ((NSArray) viewDict.objectForKey("Layers")).getArray();
                    for (NSObject layerDict : viewLayers) {
                        NSDictionary viewLayerDict = (NSDictionary) layerDict;
                        NSString submenuLayerName = (NSString) viewLayerDict.objectForKey("Name");
                        menu.add(submenuLayerName.toString());

                    }
                }



            }


        }
        catch (Exception e){

        }


        return super.onCreateOptionsMenu(menu);
    }

    @Override public boolean onTouchEvent(MotionEvent me){

        SGD.onTouchEvent(me);
        return gestureWatch.onTouchEvent(me);
    }

    @Override protected void onPause() {
        super.onPause();
        Log.i(TAG, "on Pause");


        Orca3DEngineLib.nativeOnPause();
        _view.setVisibility(View.GONE);


    }

    @Override protected void onResume() {
        super.onResume();
        Log.i(TAG,"on Resume");

        Orca3DEngineLib.nativeOnResume();
        _view.setVisibility(View.VISIBLE);

    }

    @Override protected void onStart() {
        super.onStart();
        Log.i(TAG, "onStart()");
        //String pathAndFile = getIntent().getExtras().getString("Model Name");
        String pathAndFile = m_Path + "/AnatomyInfo.plist";
        InputStream is = null;
        try {
            is = getAssets().open(pathAndFile);
            try {
                m_rootDict = (NSDictionary) PropertyListParser.parse(is);
            }
            catch (Exception ex)
            {

            }
            loadModel(m_rootDict);
        } catch (IOException e) {
            e.printStackTrace();
        }


    }

    @Override protected void onStop() {
        super.onStop();
        Log.i(TAG, "onStop()");
        Orca3DEngineLib.nativeOnStop();
    }


    public void onWindowFocusChanged(boolean hasFocus){
        //Log.i(TAG, "window was changed");
        m_WindowFocus = hasFocus;
    }



    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.i(TAG, "surface Changed");
        Orca3DEngineLib.nativeSetSurface(holder.getSurface());
        while (!Orca3DEngineLib.doneInit()) {
            try {
                Thread.sleep(50, 0);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

            createResources(m_rootDict);
            Orca3DEngineLib.resourcesLoaded();

    }

    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "surface Created");

    }
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG,"surface Destroyed");

        //Orca3DEngineLib.nativeSetSurface(null);
    }


    public void onBackPressed () {
//

        super.onBackPressed();
        //GL2JNILib.nativeOnPause();
    }
    




    @Override
    public boolean onDown(MotionEvent e) {
        if(m_WindowFocus) {
            Log.d(DEBUG_TAG, " On Down");
            twoFingerStart = false;
            zoomMode = false;
        }
            return false;

    }

    @Override
    public void onShowPress(MotionEvent e) {

    }

    @Override
    public boolean onSingleTapUp(MotionEvent e) {
        Log.d(DEBUG_TAG , " on single tap");
        return false;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent e) {
        return false;
    }

    @Override
    public boolean onDoubleTap(MotionEvent e) {
        return false;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent event) {
        Log.d(DEBUG_TAG, "onDoubleTapEvent: " + event.toString());
        Orca3DEngineLib.setTouchReset();
        zoomScale = 1;
        return true;
    }

    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {


//        long eventtime = e1.getEventTime();
//        long currentTime = uptimeMillis();
//        long time = currentTime - eventtime;
//        Log.d(DEBUG_TAG , "on scroll " + time);

        int pointerCount = e2.getPointerCount();
        if (pointerCount == 2)
        {

                Orca3DEngineLib.setTouchPan(distanceX / 10000, -distanceY / 10000);

        }
        else
        Orca3DEngineLib.setTouchRotate(distanceX / 500, distanceY / 500);
        return false;
    }

    @Override
    public void onLongPress(MotionEvent e) {
        Log.d(DEBUG_TAG , "Long Press");


    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        Log.d(DEBUG_TAG , " On fling");
        return false;
    }




    public boolean onOptionsItemSelected(MenuItem item){

        List<String> turnLayersOn = new ArrayList<String>();

        //String id = (String) item.getTitle();
        //String viewLayerName = getIntent().getExtras().getString("Layer Name");
        NSObject[] viewsArray = ((NSArray)m_rootDict.objectForKey("Views")).getArray();
        for(NSObject viewObject:viewsArray){
            NSDictionary viewDict = (NSDictionary)viewObject;
            NSString viewname = (NSString)viewDict.objectForKey("Name");
            String viewNameString = viewname.toString();
            if(viewNameString.equals(m_View)){
                NSDictionary initialState = (NSDictionary)viewDict.objectForKey("InitialState");
                String curLayer = String.valueOf(item);

                NSObject[] layersArray = ((NSArray)viewDict.objectForKey("Layers")).getArray();
                for(NSObject layerObject:layersArray){
                    NSDictionary layerDict = (NSDictionary)layerObject;
                    NSString layerName = (NSString)layerDict.objectForKey("Name");
                    String layerNameString = layerName.toString();
                    if(layerNameString.equals(curLayer)){
                        NSObject[] showLayersArray = ((NSArray)layerDict.objectForKey("ShowLayers")).getArray();
                        for(NSObject showLayer:showLayersArray){
                            NSString showLayerName = (NSString)showLayer;

                            turnLayersOn.add(showLayerName.toString());
                        }
                    }

                }
            }
        }
        showLayers(m_hideLayerList,turnLayersOn);
        return true;
    }

    private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
           // if (zoomMode == true) {

             //   float info = detector.getScaleFactor();
             //   Log.d(TAG, "siz of scale :" + info);

            if(m_WindowFocus) {
                zoomScale *= detector.getScaleFactor();
                //   Log.d(TAG, "zoomscale value: " + zoomScale);
                Orca3DEngineLib.setTouchZoom(zoomScale);
            }

           // }
            return true;
        }
    }



    public boolean createResources(NSDictionary rootDict)
    {
        try
        {

            Log.d(DEBUG_TAG, "Create Resources");

            //NSDictionary rootDict = (NSDictionary) PropertyListParser.parse(is);
            //String podFile = rootDict.objectForKey("podFile").toString();
            //String fxFile = rootDict.objectForKey("fxFile").toString();
            //GL2JNILib.loadModel("", podFile, fxFile);
            NSObject[] layers = ((NSArray) rootDict.objectForKey("Layers")).getArray();
            for (NSObject dict : layers)
            {
                //if (dict.getClass().equals(NSDictionary.class)) {
                NSDictionary layerDict = (NSDictionary)dict;

                String layerName = layerDict.objectForKey("name").toString();
                m_hideLayerList.add(layerName);
                Orca3DEngineLib.addLayer(layerName);

                NSNumber setVisible = (NSNumber)layerDict.objectForKey("visible");
                Orca3DEngineLib.setLayerVisible(layerName, setVisible.boolValue());
                NSNumber setLabelVisible = (NSNumber)layerDict.objectForKey("labelsVisible");
                if(setLabelVisible!=null) {
                    Orca3DEngineLib.setLabelsVisible(layerName, setLabelVisible.boolValue());
                }
                NSObject checkSubstringforNull = layerDict.objectForKey("nodeNameSubstrings");
                if(checkSubstringforNull!=null) {
                    NSObject[] substrings = ((NSArray) layerDict.objectForKey("nodeNameSubstrings")).getArray();
                    for (NSObject convertSub : substrings) {
                        NSString sub = (NSString) convertSub;
                        Orca3DEngineLib.addNodeNameSubstringForLayer(layerName, sub.toString());
                    }
                }
                NSObject checkExcludeforNull = layerDict.objectForKey("excludedNodeNameSubstrings");
                if(checkExcludeforNull!=null) {
                    NSObject[] excludedsubstrings = ((NSArray) layerDict.objectForKey("excludeNodeNameSubstrings")).getArray();
                    for (NSObject convertexSub : excludedsubstrings) {
                        NSString exsub = (NSString) convertexSub;
                        Orca3DEngineLib.excludeNodeNameSubstringForLayer(layerName, exsub.toString());
                    }
                }
            }
            NSObject labels = (rootDict.objectForKey("Labels"));
            if(labels!=null)
            {
                NSDictionary labelDict = (NSDictionary)labels;
                NSNumber scaleValue = (NSNumber)labelDict.objectForKey("scale");
                Orca3DEngineLib.setLabelScale(scaleValue.floatValue());
            }
            NSObject alpha = rootDict.objectForKey("AlphaBlend");
            if(alpha!=null)
            {
                NSDictionary alphaDict = (NSDictionary) alpha;
                NSNumber transSkin = (NSNumber) alphaDict.objectForKey("TransparentSkin");
                if(transSkin!=null) {
                    Orca3DEngineLib.enableAlphaBlend(true, transSkin.boolValue());
                }
                NSObject[] subStrings = ((NSArray)alphaDict.objectForKey("nodeNameSubstrings")).getArray();
                for(NSObject convertSub:subStrings)
                {
                    NSString sub = (NSString)convertSub;
                    Orca3DEngineLib.addAlphaNodeNameSubstringForAllLayers(sub.toString());
                }
            }
            Orca3DEngineLib.assignLayersAndAlphaToNodes();
            NSObject zoom = rootDict.objectForKey("Zoom");
            if(zoom!=null){
                NSDictionary zoomDict = (NSDictionary) zoom;
                NSNumber zoomScale =(NSNumber) zoomDict.objectForKey("scale");
                Orca3DEngineLib.setZoomScale(zoomScale.floatValue());
                NSNumber zoomMin = (NSNumber) zoomDict.objectForKey("min");
                NSNumber zoomMax = (NSNumber) zoomDict.objectForKey("max");
                Orca3DEngineLib.setZoomMin(zoomMin.floatValue(), zoomMax.floatValue());
            }
            NSObject rotate = rootDict.objectForKey("Rotate");
            if(rotate!=null)
            {
                NSDictionary rotateDict = (NSDictionary) rotate;
                NSObject[] offset = ((NSArray) rotateDict.objectForKey("Offset")).getArray();
                NSNumber xVal = (NSNumber)offset[0];
                NSNumber yVal = (NSNumber)offset[1];
                NSNumber zVal = (NSNumber)offset[2];
                Orca3DEngineLib.setRotationOffsetAxis(xVal.floatValue(), yVal.floatValue(), zVal.floatValue());
                NSObject center = rotateDict.objectForKey("Center");
                if(center!=null) {
                    NSObject[] centerArray = ((NSArray) rotateDict.objectForKey("Center")).getArray();
                    NSNumber xCenter = (NSNumber) centerArray[0];
                    NSNumber yCenter = (NSNumber) centerArray[1];
                    NSNumber zCenter = (NSNumber) centerArray[2];
                    Orca3DEngineLib.setWorldCenterOffset(xCenter.floatValue(), yCenter.floatValue(), zCenter.floatValue());
                }
            }
            NSObject pan = rootDict.objectForKey("Pan");
            if(pan!=null)
            {
                NSDictionary panDict = (NSDictionary)pan;
                NSNumber panScale = (NSNumber)panDict.objectForKey("scale");
                Orca3DEngineLib.setPanScale(panScale.floatValue());
            }
            NSNumber lightScale = (NSNumber)rootDict.objectForKey("LightScale");
            if(lightScale!=null)
            {
                Orca3DEngineLib.setLightScale(lightScale.floatValue());

            }
            NSObject animCheck = rootDict.objectForKey("Animations");
            if(animCheck!=null)
            {
                NSObject[] animations = ((NSArray)rootDict.objectForKey("Animations")).getArray();
                for(NSObject anim:animations)
                {
                    NSDictionary animDict = (NSDictionary) anim;
                    NSString name = (NSString)animDict.objectForKey("name");
                    m_animationList.add(name.toString());
                    NSNumber startFrame = (NSNumber)animDict.objectForKey("startFrame");
                    NSNumber endFrame = (NSNumber)animDict.objectForKey("endFrame");
                    NSNumber oscillates = (NSNumber)animDict.objectForKey("oscillates");
                    boolean oscillatesVal = false;
                    if(oscillates!=null)
                        oscillatesVal = true;
                    NSNumber active = (NSNumber)animDict.objectForKey("active");
                    NSNumber timeBased = (NSNumber)animDict.objectForKey("timeBased");
                    if(timeBased.boolValue())
                    {
                        NSNumber duration = (NSNumber)animDict.objectForKey("duration");
                        NSNumber repeats = (NSNumber)animDict.objectForKey("repeats");
                        boolean repeatsVal = false;
                        if(repeats!=null)
                            repeatsVal = true;

                        Orca3DEngineLib.addTimeAnimation(name.toString(), duration.floatValue(), repeatsVal, startFrame.intValue(), endFrame.intValue(), oscillatesVal);

                        if(active.boolValue())
                        {
                             Orca3DEngineLib.activateAnimations(name.toString(), true);
                        }
                    }
                    else
                    {
                        Orca3DEngineLib.addControl1DAnimations(name.toString(), startFrame.intValue(), endFrame.intValue(), oscillatesVal);
                    }
                }
            }




            //set the view up
            //find the initial state of the view
            List<String> showLayerList = new ArrayList<String>();
            animationsOff(m_animationList);
            //String viewLayerName = getIntent().getExtras().getString("Layer Name");
            NSObject[] viewsArray = ((NSArray)m_rootDict.objectForKey("Views")).getArray();
            for(NSObject viewObject:viewsArray){
                NSDictionary viewDict = (NSDictionary)viewObject;
                NSString viewname = (NSString)viewDict.objectForKey("Name");
                String viewNameString = viewname.toString();
                if(viewNameString.equals(m_View)){
                    NSDictionary initialState = (NSDictionary)viewDict.objectForKey("InitialState");
                    NSDictionary circAnimationsArray = (NSDictionary)viewDict.objectForKey("CircularAnimation");
                    if(circAnimationsArray!=null)
                    {
                        NSString animation = (NSString)initialState.objectForKey("curViewMode");
                        Orca3DEngineLib.activateAnimations(animation.toString(), true);
                    }
                    else {
                        if (initialState.count() > 2) {
                            NSObject[] animationsArray = ((NSArray) initialState.objectForKey("animations")).getArray();
                            if (animationsArray != null) {
                                for (NSObject animation : animationsArray) {
                                    String animName = animation.toString();
                                    Orca3DEngineLib.activateAnimations(animName, true);
                                }
                            }
                        }
                    }
                    NSString curLayer = (NSString)initialState.objectForKey("curLayer");
                    NSObject[] layersArray = ((NSArray)viewDict.objectForKey("Layers")).getArray();
                    for(NSObject layerObject:layersArray){
                        NSDictionary layerDict = (NSDictionary)layerObject;
                        NSString layerName = (NSString)layerDict.objectForKey("Name");
                        if(layerName.equals(curLayer)){
                            NSObject[] showLayersArray = ((NSArray)layerDict.objectForKey("ShowLayers")).getArray();
                            for(NSObject showLayer:showLayersArray){
                                NSString showLayerName = (NSString)showLayer;

                                showLayerList.add(showLayerName.toString());
                            }
                        }

                    }
                }
            }
            showLayers(m_hideLayerList,showLayerList);
            Log.d(DEBUG_TAG,"resources Created.");
            float width = _view.getWidth();
            float height = _view.getHeight();
            Orca3DEngineLib.setSize(width, height);
        }
        catch(Exception ex){
            Log.i(TAG, "failed to create resources");
            return false;
        }
        return true;
    }

    public void loadModel(NSDictionary rootDict)
    {
        try {

            String podFile = rootDict.objectForKey("podFile").toString();
            String fxFile = rootDict.objectForKey("fxFile").toString();
            String path = m_Path;
            path = path +"/";
            Orca3DEngineLib.nativeOnStart(path, podFile, fxFile);
        }
        catch (Exception ex){

        }


    }
    public void showLayers(List<String> hideLayerList, List<String> showLayerList)
    {
        hideAllLayers(hideLayerList);
        for(String s : showLayerList) {
            Orca3DEngineLib.setLayerVisible(s, true);
        }

    }
    public void hideAllLayers(List<String>hideLayerList){

        for(String s: hideLayerList)
        {
            Orca3DEngineLib.setLayerVisible(s, false);
        }
    }

    public void animationsOff(List<String> animationList)
    {
        for(String s: animationList)
        {
            Orca3DEngineLib.activateAnimations(s, false);
        }
    }

}
