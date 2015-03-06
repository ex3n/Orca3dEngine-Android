//
//  Orca3dScene.h
//  Orca3D
//
//  Created by Gary Robinson on 12/2/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#ifndef __Orca3D__Orca3dScene__
#define __Orca3D__Orca3dScene__

#include <iostream>
#include "OGLES2Tools.h"
#include "Orca3dLayer.h"
#include "Orca3dAnimation.h"
//using namespace std;

#define NUM_LIGHTS 3

class Orca3dScene
{
public:
    Orca3dScene();
    ~Orca3dScene();
    
    
    bool setPodFile(const char *rootDir, const char *podFile);
    CPVRTModelPOD* getModelPOD();
    
    PVRTVec4 getLightAt(int index);
    void setLightScale(PVRTVec3 scale);
    void setCameraIndex(int index);
    
    void homePosition(float animationDuration);
    void setAspectRatio(float width, float height);
    float getWidth();
    float getHeight();
    
    //point of view into scene
    float getZoom();
    void setZoom(float zoom);
    void setZoomLimits(float minZoom, float maxZoom);
    void setZoomScale(float scale);
    
    void setPanScale(float scale);
    void pan(PVRTVec4 pan);
    
    void rotate(float xAxisAngle, float yAxisAngle, float zAxisAngle);
    void setRotationOffset(float xAxisAngle, float yAxisAngle, float zAxisAngle);
    void setRotationCenter(PVRTVec3 center);
    void setWorldCenterOffset(PVRTVec3 offset);
    void setMatricesForCurrentFrame();
    
    PVRTVec4 getEyePos();
    PVRTMat4 getRotationMatrix();
    PVRTMat4 getProjectionViewRot();
    
    //layers
    Orca3dLayer* addLayer(const char *layerName);
    void addNodeNameSubstringForLayer(const char *layerName, const char *substring);
    void excludeNodeNameSubstringFromLayer(const char *layerName, const char *substring);
    void addAlphaNodeNameSubstring(const char *substring);
    void assignLayersAndAlphaToNodes();
    void clearVisibleLayers();
    void setLayerVisible(const char *layerName, bool visible);
    bool isLayerVisible(const char *layerName);
    void setLayerHighlighted(const char *layerName, bool highlighted);
    
    int getNumLayers();
    Orca3dLayer* getLayerAt(int index);
    
    //labels
    bool getLabelsVisible(const char *layerName);
    void setLabelsVisible(bool visible, const char *layerName);
    void pickedNode(int nodeIndex);
	void pickLabelNamed(const char * labelName);
	PVRTMat4 getLabelScaleMatrix();
    void setLabelScale(float scale);
    void updateLabelsForCurrentFrame();
	void getPickedLabelInfo(string &name, float &viewX, float &viewY);
    void getLabelNamesForLayer(const char *layerName, std::vector<std::string>* labelNames);
    
    //animations
    void addTimeAnimation(string name, float duration, bool repeats, int startFrame, int endFrame, bool oscillates);
    void addControl1DAnimation(string name, int startFrame, int endFrame, bool oscillates);
    void addControl2DAnimation(string name, string type);
    
    void activateAnimation(string name, bool activate);
    
    void processTimeAnimations(float duration);
    void setAnimationProgress(string name, float progress);
    void setAnimation2DProgress(string name, float progress1, float progress2);
    
    float getActiveAnimationProgress();
    
private:
    void closePodFile();
	bool getLabelNameForNodeIndex(int nodeIndex, string& labelName);
    Orca3dLayer* getLayerNamed(string name);
    Orca3dAnimation* getAnimationNamed(string name);
    void Quaternion3DMakeWithSLERP(PVRTQUATERNION* result, PVRTQUATERNION* start, PVRTQUATERNION* finish, float progress);
    void GetViewPositionFromWorldPosition(PVRTVec4 worldPosition, float *pViewX, float *pViewY);
    
    CPVRTModelPOD* m_pModelPOD;
    PVRTVec4 m_lights[NUM_LIGHTS];
    int m_nCurCameraIndex;
    
    float m_fWidth;
    float m_fHeight;
    float m_fAspectRatio;
    
    float m_fZoom;
    float m_fMaxZoom;
    float m_fMinZoom;
    float m_fZoomScale;
    
    PVRTVec4 m_pan;
    float m_panScale;
    
    PVRTMat4 m_rotationOffset;
    PVRTQUATERNION m_rotationQuaternion;
    PVRTMat4 m_worldCenterOffset;
    PVRTVec3 m_pivot;
    PVRTVec4 m_eyePos;
    
    PVRTMat4 m_viewMatrix;
    PVRTMat4 m_rotationMatrix;
    PVRTMat4 m_projectionViewRot;
    
    PVRTMat4 m_labelScaleMatrix;
    float m_fLabelScale;
    int m_nPickedLabelIndex;
    string m_strPickedLabelName;
    float m_fLabelViewPositionX;
    float m_fLabelViewPositionY;
    
    
    vector<Orca3dLayer*> m_Layers;
    
    
    vector<Orca3dAnimation*> m_vAnimations;
    Orca3dAnimation* m_pHomeAnimation;
    
    bool m_bAnimatingHome;
    PVRTQUATERNION m_beginAnimationQuaternion;
    PVRTVec4 m_beginAnimationPan;
    float m_fBeginAnimationZoom;
    
};

#endif /* defined(__Orca3D__Orca3dScene__) */