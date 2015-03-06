//
//  Orca3dController.h
//  Orca3D
//
//  Created by Gary Robinson on 12/2/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#ifndef __Orca3D__Orca3dController__
#define __Orca3D__Orca3dController__

#include <iostream>
#include "Orca3dRenderer.h"
#include "Orca3dScene.h"

class Orca3dController
{
public:
    Orca3dController();
    ~Orca3dController();
    
    bool isInitialized();
    
    //scene
    void setModelFiles(const char *rootDir, const char *podFile, const char *fxFile);
    
    void setViewSize(float width, float height);
    float getViewWidth();
    float getViewHeight();
    
    void setLightScale(float scale);
    
    void homePosition(float animationDuration);
    
    void setPanScale(float scale);
    void pan(float panX, float panY);
    
    void setRotationOffset(float xAxisAngle, float yAxisAngle, float zAxisAngle);
    void setWorldCenterOffset(float x, float y, float z);
    void rotate(float xAxisAngle, float yAxisAngle, float zAxisAngle);
    
    float getZoom();
    void setZoom(float scale);
    void setZoomLimits(float min, float max);
    void setZoomScale(float scale);
    
    //layers, labels, and alpha
    void addLayer(const char *layerName);
    void addNodeNameSubstringForLayer(const char *layerName, const char *substring);
    void excludeNodeNameSubstringFromLayer(const char *layerName, const char *substring);
    void addAlphaNodeNameSubstringForAllLayers(const char *substring);
    void assignLayersAndAlphaToNodes();
    
    void setLayerVisible(const char *layerName, bool visible);
    void setLayerHighlighted(const char *layerName, bool highlighted);
    
    void setLabelsVisible(bool visible, const char *layerName);
    void setLabelScale(float scale);
    void pickLabelAtViewPosition(float viewX, float viewY);
	void pickLabelNamed(const char *labelName);
    bool getPickedLabelInfo(char **name, float *pViewX, float *pViewY);
    void getLabelNamesForLayer(const char *layerName, std::vector<std::string>* labelNames);
    
    //animations
    void activateAnimation(const char * name, bool activate);
    void addTimeAnimation(const char * name, float duration, bool repeats, int startFrame, int endFrame, bool oscillates);
    void addControl1DAnimation(const char *  name, int startFrame, int endFrame, bool oscillates);
    void setAnimationProgress(const char * name, float progress);
    void addControl2DAnimation(const char *  name, const char *  type);
    void setAnimation2DProgress(const char * name, float progress1, float progress2);
    
    //rendering
    void enableAlphaBlend(bool enabled, bool transparentSkin);
    void renderFrame(float timeSinceLastFrame);
    GLubyte *renderToImageData(int bufferWidth, int bufferHeight, int dataLength);
    
private:
    bool m_initialized; //initialized after setModelFiles
    
    Orca3dRenderer *m_pRenderer;
    Orca3dScene *m_pScene;
};

#endif /* defined(__Orca3D__Orca3dController__) */