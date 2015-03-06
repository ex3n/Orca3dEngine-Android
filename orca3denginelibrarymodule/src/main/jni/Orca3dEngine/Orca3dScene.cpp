//
//  Orca3dScene.cpp
//  Orca3D
//
//  Created by Gary Robinson on 12/2/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#include "Orca3dScene.h"
#include "logger.h"

#define LOG_TAG "Orca3dScene"


Orca3dScene::Orca3dScene()
{
    m_pModelPOD = NULL;
    
    m_nCurCameraIndex = 0;
    
    m_fZoom = 1.0;
    m_fMaxZoom = 10.0;
    m_fMinZoom = 0.1f;
    m_fZoomScale = 1.0;
    
    m_pan = PVRTVec4(0.,0.,0.,1.);
    m_panScale = 10.0;
    
    PVRTMatrixIdentityF(m_rotationOffset);
    PVRTMatrixQuaternionIdentityF(m_rotationQuaternion);
    
    m_pivot = PVRTVec3(0.,0.,0.); //PVRTVec3(0.,-5.,0.);
    PVRTMatrixTranslationF(m_worldCenterOffset, 0., 0., 0.);
    
    PVRTMatrixIdentityF(m_labelScaleMatrix);
    m_fLabelScale = 5.0;
    m_nPickedLabelIndex = -1;
    m_strPickedLabelName = "";
    
    m_bAnimatingHome = false;
    m_pHomeAnimation = NULL;
}

Orca3dScene::~Orca3dScene()
{
    closePodFile();
}

void Orca3dScene::closePodFile()
{
    m_Layers.clear();
    m_vAnimations.clear();
    if(m_pHomeAnimation)
    {
        delete m_pHomeAnimation;
        m_pHomeAnimation = NULL;
    }
    
    if(m_pModelPOD)
    {
        m_pModelPOD->Destroy();
        delete m_pModelPOD;
        m_pModelPOD = NULL;
    }
}

bool Orca3dScene::setPodFile(const char *rootDir, const char *podFile)
{
    closePodFile();
    LOG_INFO("getting ready to Set Model");
    m_pModelPOD = new CPVRTModelPOD();
    CPVRTResourceFile::SetReadPath(rootDir);
    LOG_INFO("read path was set to %s", rootDir);
    if(m_pModelPOD->ReadFromFile(podFile) != PVR_SUCCESS)
    {
        LOG_INFO("failed to read pod file: %s\n", podFile);
        closePodFile();
        return false;
    }
    if(m_pModelPOD->nNumCamera == 0)
    {
        closePodFile();
        LOG_INFO("the pod file doesn't have a camera\n");
        return false;
    }
    LOG_INFO("the file was set.");
    // initialize lights
    int numLights = m_pModelPOD->nNumLight;
    if(numLights > NUM_LIGHTS)
        numLights = NUM_LIGHTS;
    
	for (int lightIndex = 0; lightIndex < numLights; lightIndex++)
	{
		//treat spot as directional .. we do not yet support spot
		if(m_pModelPOD->pLight[lightIndex].eType ==  ePODDirectional || m_pModelPOD->pLight[lightIndex].eType ==  ePODSpot)
		{
			m_lights[lightIndex] = m_pModelPOD->GetLightDirection( lightIndex ).normalize();
			m_lights[lightIndex].w = -1.0;
     	}
		else
        {
            printf("WARNING: light found that isn't spot or directional\n");
			m_lights[lightIndex] = m_pModelPOD->GetLightPosition( lightIndex );
        }
	}
    
    return true;
}

CPVRTModelPOD* Orca3dScene::getModelPOD()
{
    return m_pModelPOD;
}

void Orca3dScene::homePosition(float animationDuration)
{
    if(animationDuration <= 0.)
    {
        m_fZoom = 1.0;
        m_pan = PVRTVec4(0.,0.,0.,1.);
        PVRTMatrixQuaternionIdentityF(m_rotationQuaternion);
    }
    else
    {
        m_beginAnimationQuaternion = m_rotationQuaternion;
        m_beginAnimationPan = m_pan;
        m_fBeginAnimationZoom = m_fZoom;
        
        m_bAnimatingHome = true;
        m_pHomeAnimation = new Orca3dAnimation();
        m_pHomeAnimation->set1DProgress(0.0);
        m_pHomeAnimation->setName("Home");
        m_pHomeAnimation->setTimeBased(animationDuration, false);
        m_pHomeAnimation->activate(true);
    }
}

PVRTVec4 Orca3dScene::getLightAt(int index)
{
    return m_lights[index];
}
void Orca3dScene::setLightScale(PVRTVec3 scale)
{
    if(m_pModelPOD->pLight)
    {
        for(int i=0; i<NUM_LIGHTS; i++)
        {
            m_pModelPOD->pLight[i].pfColour[0] *= scale.x;
            m_pModelPOD->pLight[i].pfColour[1] *= scale.y;
            m_pModelPOD->pLight[i].pfColour[2] *= scale.z;
        }
    }
}

void Orca3dScene::setCameraIndex(int index)
{
    m_nCurCameraIndex = index;
}

void Orca3dScene::setZoomLimits(float minZoom, float maxZoom)
{
    m_fMinZoom = minZoom;
    m_fMaxZoom = maxZoom;
}

void Orca3dScene::setZoomScale(float scale)
{
    m_fZoomScale = scale;
}

float Orca3dScene::getZoom()
{
    return m_fZoom;
}

void Orca3dScene::setZoom(float zoom)
{
    if(m_bAnimatingHome)
        return;
    
    if(zoom > m_fMaxZoom)
        m_fZoom = m_fMaxZoom;
    else if(zoom < m_fMinZoom)
        m_fZoom = m_fMinZoom;
    else
        m_fZoom = zoom;
}

void Orca3dScene::setPanScale(float scale)
{
    m_panScale = scale;
}

void Orca3dScene::pan(PVRTVec4 pan)
{
    if(m_bAnimatingHome)
        return;
    
    pan = m_rotationMatrix.inverse() * pan;
    float zoom = m_fZoom / m_fZoomScale;
    pan /= zoom;
    
    m_pan.x += pan.x * m_panScale;
    m_pan.y += pan.y * m_panScale;
    m_pan.z += pan.z * m_panScale;
    
}

void Orca3dScene::setAspectRatio(float width, float height)
{
    m_fWidth = width;
    m_fHeight = height;
    m_fAspectRatio = m_fWidth / m_fHeight;
}

float Orca3dScene::getWidth()
{
    return m_fWidth;
}

float Orca3dScene::getHeight()
{
    return m_fHeight;
}

void Orca3dScene::rotate(float xAxisAngle, float yAxisAngle, float zAxisAngle)
{
    if(m_bAnimatingHome)
        return;
    
    PVRTQUATERNIONf rot;
    PVRTVec3 xAxis(1.0, 0.0, 0.0);
    PVRTMatrixQuaternionRotationAxisF(rot, xAxis, xAxisAngle);
    PVRTMatrixQuaternionMultiplyF(m_rotationQuaternion, m_rotationQuaternion, rot);
    
    PVRTVec3 yAxis(0.0, 1.0, 0.0);
    PVRTMatrixQuaternionRotationAxisF(rot, yAxis, yAxisAngle);
    PVRTMatrixQuaternionMultiplyF(m_rotationQuaternion, m_rotationQuaternion, rot);
    
    PVRTVec3 zAxis(0.0, 0.0, 1.0);
    PVRTMatrixQuaternionRotationAxisF(rot, zAxis, zAxisAngle);
    PVRTMatrixQuaternionMultiplyF(m_rotationQuaternion, m_rotationQuaternion, rot);
}

void Orca3dScene::setRotationOffset(float xAxisAngle, float yAxisAngle, float zAxisAngle)
{
    PVRTMatrixIdentityF(m_rotationOffset);
    
    PVRTMat4 rot;
    PVRTMatrixRotationXF(rot, xAxisAngle);
    PVRTMatrixMultiplyF(m_rotationOffset, m_rotationOffset, rot);
    
    PVRTMatrixRotationYF(rot, yAxisAngle);
    PVRTMatrixMultiplyF(m_rotationOffset, m_rotationOffset, rot);
    
    PVRTMatrixRotationZF(rot, zAxisAngle);
    PVRTMatrixMultiplyF(m_rotationOffset, m_rotationOffset, rot);
}

void Orca3dScene::setRotationCenter(PVRTVec3 center)
{
    //i want to get this to work. it's always 0 right now
}

void Orca3dScene::setWorldCenterOffset(PVRTVec3 offset)
{
    PVRTMatrixTranslationF(m_worldCenterOffset, offset.x, offset.y, offset.z);
}

void Orca3dScene::setMatricesForCurrentFrame()
{
    //get the rotation matrix
    PVRTMatrixRotationQuaternionF(m_rotationMatrix, m_rotationQuaternion);
    
    //apply the rotation offset for this model
    PVRTMatrixMultiplyF(m_rotationMatrix, m_rotationOffset, m_rotationMatrix);
    
    //get the scale matrix (zoom)
    PVRTMat4 scaleMatrix;
    PVRTMatrixIdentityF(scaleMatrix);
    float zoom = m_fZoom / m_fZoomScale;
    PVRTMatrixScalingF(scaleMatrix, zoom, zoom, zoom);
    
    //get the scale for the pins at the current zoom level
    float labelScale = m_fLabelScale / zoom;
    if(labelScale < 1.0f)
        labelScale = 1.0f;
    PVRTMatrixScalingF(m_labelScaleMatrix, labelScale, labelScale, labelScale);
    
    
    // Calculate the projection matrix
    float fov;
    PVRTVec3 vFrom, vTo, vUp(0.0f, 1.0f, 0.0f);
	if(m_pModelPOD->pCamera[m_nCurCameraIndex].nIdxTarget != -1) // Does the camera have a target?
		fov = m_pModelPOD->GetCameraPos( vFrom, vTo, m_nCurCameraIndex); // vTo is taken from the target node
	else
		fov = m_pModelPOD->GetCamera( vFrom, vTo, vUp, m_nCurCameraIndex); // vTo is calculated from the rotation

    float farVal = m_pModelPOD->pCamera[m_nCurCameraIndex].fFar;

       // LOG_INFO("far clipping plane %f", farVal);
	PVRTMat4 projection = PVRTMat4::PerspectiveFovRH(fov, m_fAspectRatio, m_pModelPOD->pCamera[m_nCurCameraIndex].fNear, m_pModelPOD->pCamera[m_nCurCameraIndex].fFar, PVRTMat4::OGL, false);
    
    //get the point in the scene to pivot around
    PVRTVec3 vPivot = vTo;
    vPivot.z = 0.;
    m_pivot = vPivot - m_pan;
    
    //get eye and lookat positions
    PVRTVec4 panIncrement = projection * m_pan;
    panIncrement /= 2;
    vTo.x += panIncrement.x;
	vTo.y += panIncrement.y;
//    vTo.z += panIncrement.z;
    vFrom.x += panIncrement.x;
	vFrom.y += panIncrement.y;
//    vFrom.z += panIncrement.z;
    
	m_viewMatrix = PVRTMat4::LookAtRH(vFrom, vTo, vUp);
    m_eyePos = PVRTVec4(vFrom.x, vFrom.y, vFrom.z, 1.0f);
    
	PVRTMat4 translationMatrix;
    PVRTMatrixTranslationF(translationMatrix, m_pivot.x, m_pivot.y, m_pivot.z);
    PVRTMat4 translationMatrixI;
	PVRTMatrixInverseF(translationMatrixI, translationMatrix);
    
    m_projectionViewRot = projection * m_viewMatrix * m_worldCenterOffset;
    m_projectionViewRot = m_projectionViewRot * translationMatrixI * scaleMatrix * translationMatrix;
    m_projectionViewRot = m_projectionViewRot * translationMatrixI * m_rotationMatrix * translationMatrix;
    m_projectionViewRot = m_projectionViewRot * translationMatrix;
    
}

PVRTVec4 Orca3dScene::getEyePos()
{
    return m_eyePos;
}

PVRTMat4 Orca3dScene::getRotationMatrix()
{
    return m_rotationMatrix;
}

PVRTMat4 Orca3dScene::getProjectionViewRot()
{
    return m_projectionViewRot;
}



Orca3dLayer* Orca3dScene::getLayerNamed(string name)
{
    for(unsigned int i=0; i<m_Layers.size(); i++)
    {
        Orca3dLayer *pLayer = m_Layers.at(i);
        if(pLayer->getName().compare(name) == 0)
            return pLayer;
    }
    return NULL;
}

Orca3dLayer* Orca3dScene::addLayer(const char *layerName)
{
    string name(layerName);
    Orca3dLayer *pLayer = getLayerNamed(name);
    if(!pLayer)
    {
        pLayer = new Orca3dLayer();
        pLayer->setName(name);
        m_Layers.push_back(pLayer);
    }
    return pLayer;
}

void Orca3dScene::addNodeNameSubstringForLayer(const char *layerName, const char *substring)
{
    string name(layerName);
    Orca3dLayer *pLayer = getLayerNamed(name);
    if(!pLayer)
        pLayer = addLayer(layerName);
    
    string s(substring);
    pLayer->addNodeNameSubstring(s);
}

void Orca3dScene::excludeNodeNameSubstringFromLayer(const char *layerName, const char *substring)
{
    string name(layerName);
    Orca3dLayer *pLayer = getLayerNamed(name);
    if(!pLayer)
        pLayer = addLayer(layerName);
    
    string s(substring);
    pLayer->excludeNodeNameSubstring(s);
}

void Orca3dScene::addAlphaNodeNameSubstring(const char *substring)
{
    string s(substring);
	for (unsigned int i = 0; i<m_Layers.size(); i++)
    {
        Orca3dLayer *pLayer = m_Layers.at(i);
        pLayer->addAlphaNodeNameSubstring(s);
    }
}

void Orca3dScene::assignLayersAndAlphaToNodes()
{
	for (unsigned int i = 0; i<m_Layers.size(); i++)
    {
        Orca3dLayer *pLayer = m_Layers.at(i);
        pLayer->generateNodeIndices(m_pModelPOD);
    }
}

void Orca3dScene::clearVisibleLayers()
{
	for (unsigned int i = 0; i<m_Layers.size(); i++)
    {
        Orca3dLayer *pLayer = m_Layers.at(i);
        pLayer->setVisible(false);
    }
}

void Orca3dScene::setLayerVisible(const char *layerName, bool visible)
{
    string name(layerName);
    Orca3dLayer *pLayer = getLayerNamed(name);
    if(pLayer)
        pLayer->setVisible(visible);
}
bool Orca3dScene::isLayerVisible(const char *layerName)
{
    string name(layerName);
    Orca3dLayer *pLayer = getLayerNamed(name);
    if(pLayer)
        return pLayer->getVisible();
    
    return false;
}

void Orca3dScene::setLayerHighlighted(const char *layerName, bool highlighted)
{
    string name(layerName);
    Orca3dLayer *pLayer = getLayerNamed(name);
    if(pLayer)
        pLayer->setHighlighted(highlighted);
}

bool Orca3dScene::getLabelsVisible(const char *layerName)
{
    string name(layerName);
    Orca3dLayer *pLayer = getLayerNamed(name);
    if(pLayer)
        return pLayer->getLabelsVisible();
    
    return false;
}
void Orca3dScene::setLabelsVisible(bool visible, const char *layerName)
{
    string name(layerName);
    Orca3dLayer *pLayer = getLayerNamed(name);
    if(pLayer)
        pLayer->setLabelsVisible(visible);
}
void Orca3dScene::pickedNode(int nodeIndex)
{
    m_nPickedLabelIndex = -1;
    m_strPickedLabelName = "";
    
    if(nodeIndex < 0)
        return;
    
	string labelName;
	if (getLabelNameForNodeIndex(nodeIndex, labelName))
	{
		m_nPickedLabelIndex = nodeIndex;
		m_strPickedLabelName = labelName;
	}
}
bool Orca3dScene::getLabelNameForNodeIndex(int nodeIndex, string& labelName)
{
	SPODNode &node = m_pModelPOD->pNode[nodeIndex];
	string nodeName(node.pszName);
	if (nodeName.find("annotation") != string::npos)
	{
		size_t pos = nodeName.find("name_");
		labelName = nodeName.substr(pos + 5);
		pos = labelName.find("_");
		while (pos != string::npos)
		{
			labelName.replace(pos, 1, " ");
			pos = labelName.find("_");
		}
		pos = labelName.find("leftParan");
		if (pos != string::npos)
			labelName.replace(pos, 9, "(");
		pos = labelName.find("rightParan");
		if (pos != string::npos)
			labelName.replace(pos, 10, ")");
        
		return true;
	}
	return false;
}
void Orca3dScene::pickLabelNamed(const char * labelName)
{
	for (int i = 0; i < getNumLayers(); i++)
	{
		Orca3dLayer *layer = getLayerAt(i);
		if (layer->getVisible())
		{
			for (int k = 0; k < layer->getNumLabelIndices(); k++)
			{
				int labelNodeIndex = layer->getLabelIndexAt(k);
				string curLabelName;
				getLabelNameForNodeIndex(labelNodeIndex, curLabelName);
				if (curLabelName.compare(labelName) == 0)
				{
					pickedNode(labelNodeIndex);
					return;
				}
			}
		}
	}
    
	pickedNode(-1);
}
PVRTMat4 Orca3dScene::getLabelScaleMatrix()
{
    return m_labelScaleMatrix;
}
void Orca3dScene::setLabelScale(float scale)
{
    m_fLabelScale = scale;
}
void Orca3dScene::updateLabelsForCurrentFrame()
{
    if(m_nPickedLabelIndex < 0)
        return;
    
    //get the world position of the currently picked label
    PVRTVec4 pinPosition = PVRTVec4(0.f,0.1f,0.f,1.f);
    SPODNode &node = m_pModelPOD->pNode[m_nPickedLabelIndex];
    SPODMesh &mesh = m_pModelPOD->pMesh[node.nIdx];
    
    if(mesh.sBoneIdx.n > 0)
    {
        //process bone positions the same as the vertex shader does
        for(int i32Batch = 0; i32Batch < mesh.sBoneBatches.nBatchCnt; ++i32Batch)
        {
            PVRTMat4 amBoneWorld[16];
            int i32Count = mesh.sBoneBatches.pnBatchBoneCnt[i32Batch];
            for(int i = 0; i < i32Count; ++i)
            {
                int i32NodeID = mesh.sBoneBatches.pnBatches[i32Batch * mesh.sBoneBatches.nBatchBoneMax + i];
                amBoneWorld[i] = m_pModelPOD->GetBoneWorldMatrix(node, m_pModelPOD->pNode[i32NodeID]);
                amBoneWorld[i] *= getLabelScaleMatrix();
            }
            
            float fBoneIdx[4], fBoneWeights[4];
            unsigned int ui32Offset = 3 * mesh.sBoneBatches.pnBatchOffset[0];
            unsigned int idx = ui32Offset;
            
            
            PVRTVertexRead((PVRTVECTOR4f*) &fBoneIdx[0], mesh.pInterleaved + (unsigned long)(mesh.sBoneIdx.pData) + (idx * mesh.sBoneIdx.nStride), mesh.sBoneIdx.eType, mesh.sBoneIdx.n);
            PVRTVertexRead((PVRTVECTOR4f*) &fBoneWeights[0], mesh.pInterleaved + (unsigned long)(mesh.sBoneWeight.pData) + (idx * mesh.sBoneWeight.nStride), mesh.sBoneWeight.eType, mesh.sBoneWeight.n);
            
            PVRTVec4 position = PVRTVec4(0.,0.,0.,0.);
            for(int bone=0; bone < i32Count; bone++)
            {
                PVRTMat4 boneMatrix = amBoneWorld[(int)(fBoneIdx[bone])];
                position += boneMatrix * pinPosition * fBoneWeights[bone];
            }
            pinPosition = position;
        }
    }
    else
    {
        PVRTMat4 curNodeWorldMatrix = m_pModelPOD->GetWorldMatrix(node);
        curNodeWorldMatrix *= getLabelScaleMatrix();
        pinPosition = curNodeWorldMatrix * pinPosition;
    }
    
    //now translate that world position to view coordinates
    GetViewPositionFromWorldPosition(pinPosition, &m_fLabelViewPositionX, &m_fLabelViewPositionY);
    
}
void Orca3dScene::GetViewPositionFromWorldPosition(PVRTVec4 worldPosition, float *pViewX, float *pViewY)
{
    PVRTVec4 screen = m_projectionViewRot * worldPosition;
    screen /= screen.w;
    //change from range(-1,1) to range(0,1)
    screen.x = (screen.x + 1.0f) / 2.0f;
    screen.y = (screen.y + 1.0f) / 2.0f;
    //change to view coordinate system
    *pViewX = screen.x * m_fWidth;
    *pViewY = m_fHeight - m_fHeight * screen.y;
}
void Orca3dScene::getPickedLabelInfo(string &name, float &viewX, float &viewY)
{
    name = m_strPickedLabelName;
    viewX = m_fLabelViewPositionX;
    viewY = m_fLabelViewPositionY;
}
void Orca3dScene::getLabelNamesForLayer(const char *layerName, std::vector<std::string>* labelNames)
{
    string name(layerName);
    Orca3dLayer *pLayer = getLayerNamed(name);
    if(pLayer)
    {
        for(int i=0; i<pLayer->getNumLabelIndices(); i++)
        {
            int labelNodeIndex = pLayer->getLabelIndexAt(i);
            string labelName;
            getLabelNameForNodeIndex(labelNodeIndex, labelName);
            
            //only add if it isn't already in there
            bool bAlreadyThere = false;
            for(int j=0; j<labelNames->size(); j++)
            {
                int compare = labelName.compare(labelNames->at(j));
                if(compare == 0)
                {
                    bAlreadyThere = true;
                    break;
                }
                else if(compare < 0)
                {
                    labelNames->insert(labelNames->begin() + j, labelName);
                    bAlreadyThere = true;
                    break;
                }
            }
            if(!bAlreadyThere)
                labelNames->push_back(labelName);
        }
    }
}



int Orca3dScene::getNumLayers()
{
    return (int)(m_Layers.size());
}
Orca3dLayer* Orca3dScene::getLayerAt(int index)
{
    if((index >= 0) && (index < getNumLayers()))
        return m_Layers.at(index);
    
    return NULL;
}



void Orca3dScene::addTimeAnimation(string name, float duration, bool repeats, int startFrame, int endFrame, bool oscillates)
{
    Orca3dAnimation *timeAnimation = new Orca3dAnimation();
    timeAnimation->setName(name);
    timeAnimation->setTimeBased(duration, repeats);
    timeAnimation->setFrames(startFrame, endFrame, oscillates);
    m_vAnimations.push_back(timeAnimation);
}
void Orca3dScene::addControl1DAnimation(string name, int startFrame, int endFrame, bool oscillates)
{
    Orca3dAnimation *control1DAnimation = new Orca3dAnimation();
    control1DAnimation->setName(name);
    control1DAnimation->set1DBased();
    control1DAnimation->setFrames(startFrame, endFrame, oscillates);
    m_vAnimations.push_back(control1DAnimation);
}
void Orca3dScene::addControl2DAnimation(string name, string type)
{
    Orca3dAnimation *control2DAnimation = new Orca3dAnimation();
    control2DAnimation->setName(name);
    control2DAnimation->set2DBased(type);
    m_vAnimations.push_back(control2DAnimation);
}
Orca3dAnimation* Orca3dScene::getAnimationNamed(string name)
{
    for(unsigned int i=0; i<m_vAnimations.size(); i++)
    {
        Orca3dAnimation *animation = m_vAnimations.at(i);
        if(name.compare(animation->getName()) == 0)
            return animation;
    }
    return NULL;
}
void Orca3dScene::activateAnimation(string name, bool activate)
{
    Orca3dAnimation *animation = getAnimationNamed(name);
    if(animation)
        animation->activate(activate);
    
}

void Orca3dScene::processTimeAnimations(float duration)
{
    if(m_bAnimatingHome)
    {
        bool stillRunning = m_pHomeAnimation->advanceProgress(duration);
        float progress = m_pHomeAnimation->getProgress();
        if(progress > 1.1)
            progress = 1.0;
        if(progress < 0.0)
            progress = 0.0;
        
        //interpolate zoom, pan, rotate back to defaults, based on progress
        PVRTQUATERNION endAnimationQuaternion;
        PVRTMatrixQuaternionIdentityF(endAnimationQuaternion);
        Quaternion3DMakeWithSLERP(&m_rotationQuaternion, &m_beginAnimationQuaternion, &endAnimationQuaternion, progress);
        
        PVRTVec4 endAnimationPan(0.,0.,0.,1.);
        m_pan = m_beginAnimationPan + (progress * (endAnimationPan - m_beginAnimationPan));
        
        m_fZoom = m_fBeginAnimationZoom + (progress * (1.0f - m_fBeginAnimationZoom));
        
        if(!stillRunning)
        {
            delete m_pHomeAnimation;
            m_pHomeAnimation = NULL;
            m_bAnimatingHome = false;
        }
    }
    
    for(unsigned int i=0; i<m_vAnimations.size(); i++)
    {
        Orca3dAnimation *animation = m_vAnimations.at(i);
        if(animation->isActive() && animation->isTimeBased())
        {
            bool stillRunning = animation->advanceProgress(duration);
            animation->applyCurrentFrameToModel(m_pModelPOD);
            if(!stillRunning)
                animation->activate(false);
            
            //only process one time animation per frame
            break;
        }
    }
}

void Orca3dScene::Quaternion3DMakeWithSLERP(PVRTQUATERNION* result, PVRTQUATERNION* start, PVRTQUATERNION* finish, float progress)
{
    float startWeight, finishWeight, difference;
    PVRTQUATERNION ret;
    
    difference = ((start->x * finish->x) + (start->y * finish->y) + (start->z * finish->z) + (start->w * finish->w));
    if ((1.f - fabs(difference)) > .01f)
    {
        float theta, oneOverSinTheta;
        
        theta = acosf(fabsf(difference));
        oneOverSinTheta = (1.f / sinf(theta));
        startWeight = (sinf(theta * (1.f - progress)) * oneOverSinTheta);
        finishWeight = (sinf(theta * progress) * oneOverSinTheta);
        if (difference < 0.f)
            startWeight = -startWeight;
    } else
    {
        startWeight = (1.f - progress);
        finishWeight = progress;
    }
    ret.x = (start->x * startWeight) + (finish->x * finishWeight);
    ret.y = (start->y * startWeight) + (finish->y * finishWeight);
    ret.z = (start->z * startWeight) + (finish->z * finishWeight);
    ret.w = (start->w * startWeight) + (finish->w * finishWeight);
    
    //normalize it
    float magnitude = sqrtf((ret.x * ret.x) +
                            (ret.y * ret.y) +
                            (ret.z * ret.z) +
                            (ret.w * ret.w));
    ret.x /= magnitude;
    ret.y /= magnitude;
    ret.z /= magnitude;
    ret.w /= magnitude;
    
    *result = ret;
}
void Orca3dScene::setAnimationProgress(string name, float progress)
{
    Orca3dAnimation *animation = getAnimationNamed(name);
    if(animation)
    {
        animation->set1DProgress(progress);
        animation->applyCurrentFrameToModel(m_pModelPOD);
    }
}
void Orca3dScene::setAnimation2DProgress(string name, float progress1, float progress2)
{
    Orca3dAnimation *animation = getAnimationNamed(name);
    if(animation)
        animation->set2DProgress(progress1, progress2, m_pModelPOD);
}

float Orca3dScene::getActiveAnimationProgress()
{
    for(unsigned int i=0; i<m_vAnimations.size(); i++)
    {
        Orca3dAnimation *animation = m_vAnimations.at(i);
        if(animation->isActive())
            return animation->getProgress();
    }
    return 0.f;
}
