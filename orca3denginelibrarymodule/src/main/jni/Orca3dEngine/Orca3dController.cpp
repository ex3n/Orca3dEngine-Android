//
//  Orca3dController.cpp
//  Orca3D
//
//  Created by Gary Robinson on 12/2/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#include "Orca3dController.h"
#include "logger.h"

#define LOG_TAG "Orca3dController"

Orca3dController::Orca3dController()
{
    m_initialized = false;
    
    m_pScene = new Orca3dScene();
    m_pRenderer = new Orca3dRenderer(m_pScene);
}

Orca3dController::~Orca3dController()
{
    if(m_pRenderer)
    {
        delete m_pRenderer;
        m_pRenderer = NULL;
    }
    if(m_pScene)
    {
        delete m_pScene;
        m_pScene = NULL;
    }
}

bool Orca3dController::isInitialized()
{
    return m_initialized;
}

void Orca3dController::setModelFiles(const char *rootDir, const char *podFile, const char *fxFile)
{
    LOG_INFO("loading model");
    LOG_INFO("root dir is %s", rootDir);
    if(!m_pScene->setPodFile(rootDir, podFile))
        return;
    LOG_INFO("Pod file Set");
    m_pRenderer->setEffectsFile(fxFile);
    LOG_INFO("effects file set");
    m_pRenderer->initMeshData();
    
    m_initialized = true;
}
void Orca3dController::setViewSize(float width, float height)
{
    m_pScene->setAspectRatio(width, height);
}
float Orca3dController::getViewWidth()
{
    return m_pScene->getWidth();
}
float Orca3dController::getViewHeight()
{
    return m_pScene->getHeight();
}
void Orca3dController::setLightScale(float scale)
{
    PVRTVec3 lightScale(scale, scale, scale);
    m_pScene->setLightScale(lightScale);
}
void Orca3dController::homePosition(float animationDuration)
{
    m_pScene->homePosition(animationDuration);
}
void Orca3dController::setZoomLimits(float min, float max)
{
    m_pScene->setZoomLimits(min, max);
}
void Orca3dController::setZoomScale(float scale)
{
    m_pScene->setZoomScale(scale);
}
void Orca3dController::setZoom(float scale)
{
    m_pScene->setZoom(scale);
}
float Orca3dController::getZoom()
{
    return m_pScene->getZoom();
}
void Orca3dController::setPanScale(float scale)
{
    m_pScene->setPanScale(scale);
}
void Orca3dController::pan(float panX, float panY)
{
    PVRTVec4 pan = PVRTVec4(panX, panY, 0., 1.0);
    m_pScene->pan(pan);
}
void Orca3dController::setRotationOffset(float xAxisAngle, float yAxisAngle, float zAxisAngle)
{
    m_pScene->setRotationOffset(xAxisAngle, yAxisAngle, zAxisAngle);
}
void Orca3dController::setWorldCenterOffset(float x, float y, float z)
{
    PVRTVec3 offset(x, y, z);
    m_pScene->setWorldCenterOffset(offset);
}
void Orca3dController::rotate(float xAxisAngle, float yAxisAngle, float zAxisAngle)
{
    m_pScene->rotate(xAxisAngle, yAxisAngle, zAxisAngle);
}


void Orca3dController::addLayer(const char *layerName)
{
    m_pScene->addLayer(layerName);
}
void Orca3dController::addNodeNameSubstringForLayer(const char *layerName, const char *substring)
{
    m_pScene->addNodeNameSubstringForLayer(layerName, substring);
}
void Orca3dController::excludeNodeNameSubstringFromLayer(const char *layerName, const char *substring)
{
    m_pScene->excludeNodeNameSubstringFromLayer(layerName, substring);
}
void Orca3dController::addAlphaNodeNameSubstringForAllLayers(const char *substring)
{
    m_pScene->addAlphaNodeNameSubstring(substring);
}
void Orca3dController::assignLayersAndAlphaToNodes()
{
    m_pScene->assignLayersAndAlphaToNodes();
}
void Orca3dController::setLayerVisible(const char *layerName, bool visible)
{
    m_pScene->setLayerVisible(layerName, visible);
}
void Orca3dController::setLayerHighlighted(const char *layerName, bool highlighted)
{
    m_pScene->setLayerHighlighted(layerName, highlighted);
}
void Orca3dController::setLabelsVisible(bool visible, const char *layerName)
{
    m_pScene->setLabelsVisible(visible, layerName);
}
void Orca3dController::setLabelScale(float scale)
{
    m_pScene->setLabelScale(scale);
}
void Orca3dController::pickLabelAtViewPosition(float viewX, float viewY)
{
    m_pRenderer->pickLabelAtViewPosition(viewX, viewY);
}
void Orca3dController::pickLabelNamed(const char *labelName)
{
	m_pScene->pickLabelNamed(labelName);
}
bool Orca3dController::getPickedLabelInfo(char **name, float *pViewX, float *pViewY)
{
    string labelName;
    float x,y;
    m_pScene->getPickedLabelInfo(labelName, x, y);
    
    if(labelName.compare("") == 0)
    {
        strcpy(*name, "");
        *pViewX = 0.f;
        *pViewY = 0.f;
        return false;
    }
    else
    {
        strcpy(*name, labelName.c_str());
        *pViewX = x;
        *pViewY = y;
        return true;
    }
}
void Orca3dController::getLabelNamesForLayer(const char *layerName, std::vector<std::string>* labelNames)
{
    m_pScene->getLabelNamesForLayer(layerName, labelNames);
}


void Orca3dController::activateAnimation(const char * name, bool activate)
{
    string sName(name);
    m_pScene->activateAnimation(sName, activate);
}
void Orca3dController::addTimeAnimation(const char * name, float duration, bool repeats, int startFrame, int endFrame, bool oscillates)
{
    string sName(name);
    m_pScene->addTimeAnimation(sName, duration, repeats, startFrame, endFrame, oscillates);
}
void Orca3dController::addControl1DAnimation(const char * name, int startFrame, int endFrame, bool oscillates)
{
    string sName(name);
    m_pScene->addControl1DAnimation(name, startFrame, endFrame, oscillates);
}
void Orca3dController::setAnimationProgress(const char * name, float progress)
{
    string sName(name);
    m_pScene->setAnimationProgress(sName, progress);
}
void Orca3dController::addControl2DAnimation(const char *  name, const char *  type)
{
    string sName(name);
    string sType(type);
    m_pScene->addControl2DAnimation(sName, sType);
}
void Orca3dController::setAnimation2DProgress(const char * name, float progress1, float progress2)
{
    string sName(name);
    m_pScene->setAnimation2DProgress(sName, progress1, progress2);
}



void Orca3dController::enableAlphaBlend(bool enabled, bool transparentSkin)
{
    m_pRenderer->enableAlphaBlend(enabled, transparentSkin);
}
void Orca3dController::renderFrame(float timeSinceLastFrame)
{
    //LOG_INFO("Start Render Frame");
    //LOG_INFO("Time Since Last Frame %f" ,timeSinceLastFrame);
//    GLenum err;
//    err = glGetError();
//    LOG_INFO("gl error = %s", err);
    m_pRenderer->renderFrame(timeSinceLastFrame);
    //LOG_INFO("finished Render Frame");
}
GLubyte *Orca3dController::renderToImageData(int bufferWidth, int bufferHeight, int dataLength)
{
    return m_pRenderer->renderToImageData(bufferWidth, bufferHeight, dataLength);
}