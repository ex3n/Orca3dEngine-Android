//
//  Orca3dEffects.cpp
//  Orca3D
//
//  Created by Gary Robinson on 12/4/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#include "Orca3dEffects.h"
#include "logger.h"

#define LOG_TAG "Orca3DEffects"
/****************************************************************************
 ** Structures
 ****************************************************************************/
//struct SPVRTPFXEffectSemantic
//{
//	const char		*p;	// String containing semantic
//	unsigned int	n;	// Application-defined semantic value
//};

/****************************************************************************
 ** Enumerations
 ****************************************************************************/

// Shader semantics recognised by this program
enum pfxSemantic
{
    pfxEFFECTSTATE = ePVRTPFX_NumSemantics,
	pfxDEVICEORIENTATION ,
	pfxNODEINDEX ,
	pfxIS_HIGHLIGHTED,
	pfxMULTIPLIED_COLOR,
    pfxIS_CLEAR_LAYER,
    pfxPICKER_INFO
};

/****************************************************************************
 ** Constants
 ****************************************************************************/
const static SPVRTPFXUniformSemantic c_psUniformSemantics[] =
{
	{ "EFFECTSTATE",			pfxEFFECTSTATE },
	{ "DEVICEORIENTATION",		pfxDEVICEORIENTATION },	//28
	{ "NODEINDEX",				pfxNODEINDEX },
	{ "IS_HIGHLIGHTED",			pfxIS_HIGHLIGHTED },
	{ "MULTIPLIED_COLOR",		pfxMULTIPLIED_COLOR },
    { "IS_CLEAR_LAYER",         pfxIS_CLEAR_LAYER },
    { "PICKER_INFO",            pfxPICKER_INFO}
};

//typedef enum{
//	effectStateModeTime,
//	effectStateModeAnimation
//}EffectStateMode;

Orca3dSEffect::Orca3dSEffect()
{
    m_pEffect = NULL;
    m_ui32UniformCount = 0;
}

Orca3dSEffect::~Orca3dSEffect()
{
    cleanup();
}

void Orca3dSEffect::cleanup()
{
    if(m_pEffect)
    {
        delete m_pEffect;
        m_pEffect = NULL;
    }
}

void Orca3dSEffect::init(CPVRTPFXParser *parser, const char *effectName, const char *filename)
{
    cleanup();
    m_pEffect = new CPVRTPFXEffect();
    
    CPVRTString error;
    if(m_pEffect->RegisterUniformSemantic(c_psUniformSemantics, sizeof(c_psUniformSemantics) / sizeof(*c_psUniformSemantics), &error) != PVR_SUCCESS)
    {
        LOG_INFO("Failed to RegisterUniformSemantic for effect %s: %s\n", effectName, error.c_str());
        return;
    }
    
    unsigned int nUnknownUniformCount;
    if(m_pEffect->Load(*parser, effectName, filename, NULL, nUnknownUniformCount, &error) != PVR_SUCCESS)
    {
        LOG_INFO("Failed to load effect %s: %s\n", effectName, error.c_str());
        return;
    }
    
    if(nUnknownUniformCount)
    {
        LOG_INFO("Unknown semantic count for effect %s: %s\n", effectName, error.c_str());
        return;
    }
}

void Orca3dSEffect::initTextures(vector<string> *pTextureNames, vector<GLuint> *pTextureLocations)
{
    const CPVRTArray<SPVRTPFXTexture>& sTex = m_pEffect->GetTextureArray();
    for(unsigned int i=0; i<sTex.GetSize(); i++)
    {
        const CPVRTStringHash *s = &sTex[i].Name;
        string texName(s->c_str());
        for(unsigned int j=0; j<pTextureNames->size(); j++)
        {
            if(texName.compare(pTextureNames->at(j)) == 0)
                m_pEffect->SetTexture(i, pTextureLocations->at(j));
        }
    }
}

void Orca3dSEffect::activate()
{
    m_pEffect->Activate();
}

unsigned int Orca3dSEffect::getNumUniforms()
{
    return m_pEffect->GetUniformArray().GetSize();
}

SPVRTPFXUniform Orca3dSEffect::getUniformAt(unsigned int index)
{
    return m_pEffect->GetUniformArray()[index];
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////



Orca3dEffects::Orca3dEffects()
{
    m_pEffectParser = NULL;
    m_nNoTexturesIndex = 0;
    m_nPickingIndex = 0;
    m_bPicking = false;
}

Orca3dEffects::~Orca3dEffects()
{
    closeEffectParser();
    
    for (unsigned int i = 0; i < m_vTextureLocations.size(); i++)
	{
		GLuint textureLoc = m_vTextureLocations.at(i);
		glDeleteTextures(1, &textureLoc);
	}
}

void Orca3dEffects::closeEffectParser()
{
    m_vSEffects.clear();
    
    if(m_pEffectParser)
    {
        delete m_pEffectParser;
        m_pEffectParser = NULL;
    }
}

string Orca3dEffects::removeExtension(string effectName)
{
    string ext(".png");
    size_t pos = effectName.rfind(ext);
    if(pos != string::npos)
        effectName = effectName.erase(pos, 4);
    
    string ext2(".pvr");
    size_t pos2 = effectName.rfind(ext2);
    if(pos2 != string::npos)
        effectName = effectName.erase(pos2, 4);
    
    return effectName;
}

void Orca3dEffects::setEffectsFile(const char * filename)
{
    m_vTextureNames.clear();
    m_vTextureLocations.clear();
    m_vEffectNames.clear();
    m_vSEffects.clear();
    
    closeEffectParser();
    LOG_INFO("start setting effects file");
    CPVRTString error;
    m_pEffectParser = new CPVRTPFXParser();
    if(m_pEffectParser->ParseFromFile(filename, &error) != PVR_SUCCESS)
    {
        LOG_INFO("failed to parse effects file: %s, %s\n", filename, error.c_str());
        return;
    }
    
    LOG_INFO("load the textures in the fx file");
    for(unsigned int i=0; i < m_pEffectParser->GetNumberTextures(); i++)
	{
        GLuint loc;
        LOG_INFO("start loading textures");
        const char * textureFileName = m_pEffectParser->GetTexture(i)->FileName.c_str();
		if(PVRTTextureLoadFromPVR(textureFileName, &loc) != PVR_SUCCESS)
        {
            LOG_INFO("Failed to load texture: %s\n", textureFileName);
			continue;
        }
        
		LOG_INFO("loading texture %s\n", textureFileName);
        string texName(textureFileName);
        texName = this->removeExtension(texName);
        m_vTextureNames.push_back(texName);
        m_vTextureLocations.push_back(loc);
	}
    
    LOG_INFO("now load the actual effects from the fx file");
    string noTexturesName("noTexturesEffect");
    string pickingName("pickingEffect");
	for (unsigned int i = 0; i < m_pEffectParser->GetNumberEffects(); i++)
    {
        Orca3dSEffect *pSEffect = new Orca3dSEffect();
        const char *pEffectName = m_pEffectParser->GetEffect(i).Name.c_str();
        pSEffect->init(m_pEffectParser, pEffectName, filename);
        pSEffect->initTextures(&m_vTextureNames, &m_vTextureLocations);
        
        string effectName(pEffectName);
        if(effectName.compare(noTexturesName) == 0)
            m_nNoTexturesIndex = i;
        else if(effectName.compare(pickingName) == 0)
            m_nPickingIndex = i;
        
        m_vEffectNames.push_back(effectName);
        m_vSEffects.push_back(pSEffect);
    }
    LOG_INFO("finished effects file load");
}

bool Orca3dEffects::curNodeHasBoneMatLoc()
{
    return m_curNodeHasBoneMatLoc;
}

bool Orca3dEffects::curNodeHasBoneMatITLoc()
{
    return m_curNodeHasBoneMatITLoc;
}

GLuint Orca3dEffects::getCurNodeBoneMatLoc()
{
    return m_boneMatrixArrayLoc;
}

GLuint Orca3dEffects::getCurNodeBoneMatITLoc()
{
    return m_boneMatrixArrayITLoc;
}

GLuint Orca3dEffects::getCurNodeWorldMatrixLoc()
{
    return m_boneWorldMatrixLoc;
}

void Orca3dEffects::setPicking(bool picking)
{
    m_bPicking = picking;
}

void Orca3dEffects::processEffects(Orca3dScene *pScene, int index, int drawPass, bool highlight, float progress, bool bIsLabel)
{
    CPVRTModelPOD* pModelPOD = pScene->getModelPOD();
    PVRTMat4 projViewRot = pScene->getProjectionViewRot();
    
    unsigned int effectIndex = 0;
    SPODNode &node = pModelPOD->pNode[index];
    SPODMaterial &Material = pModelPOD->pMaterial[node.nIdxMaterial];
    SPODMesh &mesh = pModelPOD->pMesh[node.nIdx];
    int boneCount = mesh.sBoneIdx.n;
    
    PVRTMat4 curNodeObjectMatrix;
    pModelPOD->GetTransformationMatrix(curNodeObjectMatrix, node);
    PVRTMat4 curNodeWorldMatrix = pModelPOD->GetWorldMatrix(node);
    if(bIsLabel)
        curNodeWorldMatrix *= pScene->getLabelScaleMatrix();
    
    m_curNodeHasBoneMatLoc = false;
    m_curNodeHasBoneMatITLoc = false;
    
    if(m_bPicking)
    {
        effectIndex = m_nPickingIndex;
    }
    else
    {
        // Get the Effect name from the node.
        string effectName = "";
        if(node.nIdxMaterial >= 0)
        {
            SPODMaterial &Material = pModelPOD->pMaterial[node.nIdxMaterial];
            if(Material.pszEffectName)
            {
                effectName = Material.pszEffectName;
            }
            else
            {
                if(Material.nIdxTexDiffuse >= 0)
                    effectName = pModelPOD->pTexture[Material.nIdxTexDiffuse].pszName;
                else
                    effectName = Material.pszName;
            }
        }
        
        //remove the extension
        effectName = removeExtension(effectName);
        
        bool bFound = false;
        for(unsigned int i=0; i<m_vEffectNames.size(); i++)
        {
            if(effectName.compare(m_vEffectNames.at(i)) == 0)
            {
                effectIndex = i;
                bFound = true;
                break;
            }
        }
        if(!bFound)
            effectIndex = m_nNoTexturesIndex;
    }
    
//    LOG_INFO("processEffect: %s node: %s\n", m_vEffectNames.at(effectIndex).c_str(), node.pszName);
    
    Orca3dSEffect *pEffect = m_vSEffects.at(effectIndex);
    pEffect->activate();
    
    
	for (unsigned int j = 0; j < pEffect->getNumUniforms(); ++j)
    {
        SPVRTPFXUniform uniform = pEffect->getUniformAt(j);
        switch(uniform.nSemantic)
        {
            case ePVRTPFX_UsPOSITION:
            {
                glVertexAttribPointer(uniform.nLocation, 3, GL_FLOAT, GL_FALSE, mesh.sVertex.nStride,  mesh.sVertex.pData);
                glEnableVertexAttribArray(uniform.nLocation);
                break;
            }
                
            case ePVRTPFX_UsNORMAL:
            {
                glVertexAttribPointer(uniform.nLocation, 3, GL_FLOAT, GL_FALSE, mesh.sNormals.nStride, mesh.sNormals.pData);
                glEnableVertexAttribArray(uniform.nLocation);
                break;
            }
                
            case ePVRTPFX_UsTANGENT:
            {
                if(mesh.nNumUVW > 0){
                    glVertexAttribPointer(uniform.nLocation, 3, GL_FLOAT, GL_FALSE, mesh.sTangents.nStride, mesh.sTangents.pData);
                    glEnableVertexAttribArray(uniform.nLocation);
                }
                break;
            }
                
            case ePVRTPFX_UsBINORMAL:
            {
                if(mesh.nNumUVW > 0){
                    glVertexAttribPointer(uniform.nLocation, 3, GL_FLOAT, GL_FALSE, mesh.sBinormals.nStride, mesh.sBinormals.pData);
                    glEnableVertexAttribArray(uniform.nLocation);
                }
                break;
            }
                
            case ePVRTPFX_UsUV:
            {
                if(mesh.nNumUVW > 0)
                {
                    int uvIndex = uniform.nIdx;
                    glVertexAttribPointer(uniform.nLocation, 2, GL_FLOAT, GL_FALSE, mesh.psUVW[uvIndex].nStride, mesh.psUVW[uvIndex].pData);
                    glEnableVertexAttribArray(uniform.nLocation);
                    uvIndex++;
                }
                break;
            }
                
            case ePVRTPFX_UsBONEINDEX:
            {
                if(boneCount> 0){
                    glVertexAttribPointer(uniform.nLocation, mesh.sBoneIdx.n, GL_UNSIGNED_BYTE, GL_FALSE, mesh.sBoneIdx.nStride, mesh.sBoneIdx.pData);
                    glEnableVertexAttribArray(uniform.nLocation);
                }
                break;
            }
                
            case ePVRTPFX_UsBONEWEIGHT:
            {
                if(boneCount> 0){
                    glVertexAttribPointer(uniform.nLocation, mesh.sBoneWeight.n, GL_FLOAT, GL_FALSE, mesh.sBoneWeight.nStride, mesh.sBoneWeight.pData);
                    glEnableVertexAttribArray(uniform.nLocation);
                }
                break;
            }
                
            case ePVRTPFX_UsWORLDVIEWPROJECTION:
            {
                PVRTMat4 curViewProjection = projViewRot * curNodeWorldMatrix;
                
                //save the shader variable location for pin annotations with bones to update as needed
                if(drawPass == 0)
                    m_boneWorldMatrixLoc = uniform.nLocation;
                
                glUniformMatrix4fv(uniform.nLocation, 1, GL_FALSE, curViewProjection.ptr());
                break;
            }
                
            case ePVRTPFX_UsVIEWPROJECTION:
            {
                PVRTMat4 viewProj = projViewRot;
                glUniformMatrix4fv(uniform.nLocation, 1, GL_FALSE, viewProj.ptr());
                break;
            }
                
            case ePVRTPFX_UsWORLD:
            {
                glUniformMatrix4fv(uniform.nLocation, 1, GL_FALSE, curNodeWorldMatrix.ptr());
                break;
            }
                
            case ePVRTPFX_UsOBJECT:
            {
                glUniformMatrix4fv(uniform.nLocation, 1, GL_FALSE, curNodeObjectMatrix.ptr());
                break;
            }
                
            case ePVRTPFX_UsOBJECTI:
            {
                glUniformMatrix4fv(uniform.nLocation, 1, GL_FALSE, curNodeObjectMatrix.inverse().ptr());
                break;
            }
                
            case ePVRTPFX_UsLIGHTPOSMODEL:
            {
                int lightNumber = uniform.nIdx;
                if(pScene->getLightAt(lightNumber).w > -0.5)
                {
                    //this is a point light
                    PVRTVec4 thisLightPos = curNodeWorldMatrix.inverse() * pScene->getRotationMatrix().inverse() * pScene->getLightAt(lightNumber);
                    glUniform4f(uniform.nLocation, thisLightPos.x, thisLightPos.y, thisLightPos.z, thisLightPos.w);
                }
                else
                {
                    //this is a directional light
                    PVRTMat3 curNodeWorldMatrix3I = PVRTMat3(curNodeWorldMatrix).inverse();
                    PVRTMat3 rotationMatrix3I = PVRTMat3(pScene->getRotationMatrix()).inverse();
                    PVRTVec3 thisLightDirection = curNodeWorldMatrix3I * rotationMatrix3I  * PVRTVec3(pScene->getLightAt(lightNumber));
                    
                    //negate the direction because our shaders use the direction to the light not the light direction
                    glUniform4f(uniform.nLocation, -thisLightDirection.x, -thisLightDirection.y, -thisLightDirection.z, -1.0);
                }
                break;
            }
                
            case ePVRTPFX_UsLIGHTCOLOR:
            {
                int lightNumber = uniform.nIdx;
                glUniform3fv(uniform.nLocation, 1, pModelPOD->pLight[lightNumber].pfColour);
                break;
            }
                
            case ePVRTPFX_UsEYEPOSMODEL:
            {
                PVRTVec4 thisEyePos = curNodeWorldMatrix.inverse() * pScene->getRotationMatrix().inverse() * pScene->getEyePos();
                //LOG_INFO("processEffect: %s node: %s\n", m_vEffectNames.at(effectIndex).c_str(), node.pszName);
                //LOG_INFO("EyePos= %f, %f, %f\n", thisEyePos.x, thisEyePos.y, thisEyePos.z);
                glUniform3f(uniform.nLocation, thisEyePos.x, thisEyePos.y, thisEyePos.z);
                break;
            }
                
            case ePVRTPFX_UsBONECOUNT:
            {
                glUniform1i(uniform.nLocation, boneCount);
                break;
            }
                
            case ePVRTPFX_UsBONEMATRIXARRAY:
            {
                m_curNodeHasBoneMatLoc = true;
                m_boneMatrixArrayLoc = uniform.nLocation;
                break;
            }
                
            case ePVRTPFX_UsBONEMATRIXARRAYIT:
            {
                m_curNodeHasBoneMatITLoc = true;
                m_boneMatrixArrayITLoc = uniform.nLocation;
                break;
            }
                
            case ePVRTPFX_UsMATERIALOPACITY:
            {
                glUniform1f(uniform.nLocation, Material.fMatOpacity);
                break;
            }
                
            case ePVRTPFX_UsMATERIALSHININESS:
            {
                glUniform1f(uniform.nLocation, Material.fMatShininess);
                break;
            }
                
            case ePVRTPFX_UsMATERIALCOLORAMBIENT:
            {
                glUniform3f(uniform.nLocation, Material.pfMatAmbient[0],  Material.pfMatAmbient[1],  Material.pfMatAmbient[2]);
                break;
            }
                
            case ePVRTPFX_UsMATERIALCOLORDIFFUSE:
            {
                glUniform3f(uniform.nLocation, Material.pfMatDiffuse[0],  Material.pfMatDiffuse[1],  Material.pfMatDiffuse[2]);
                break;
            }
                
            case ePVRTPFX_UsMATERIALCOLORSPECULAR:
            {
                glUniform3f(uniform.nLocation, Material.pfMatSpecular[0],  Material.pfMatSpecular[1],  Material.pfMatSpecular[2]);
                break;
            }
                
            case pfxEFFECTSTATE:
            {
                glUniform3f(uniform.nLocation, 1.0, 1.0, progress);
                break;
            }
                
            case ePVRTPFX_UsANIMATION:
            {
                glUniform1f(uniform.nLocation, progress);
                break;
            }
                
            case ePVRTPFX_UsTEXTURE:
            {
                unsigned int textureUnit = uniform.nIdx;
                glUniform1i(uniform.nLocation, textureUnit);
                break;
            }
                
            case pfxDEVICEORIENTATION:
            {
                //                BOOL landRight  = (self.cameraMode && (pass == 0) && ([UIApplication sharedApplication].statusBarOrientation == UIDeviceOrientationLandscapeRight));
                //                BOOL landLeft   = (self.cameraMode && (pass == 0) && ([UIApplication sharedApplication].statusBarOrientation == UIDeviceOrientationLandscapeLeft));
                //                BOOL portrait   = (self.cameraMode && (pass == 0) && ([UIApplication sharedApplication].statusBarOrientation == UIDeviceOrientationPortrait));
                //                BOOL upsideDown = (self.cameraMode && (pass == 0) && ([UIApplication sharedApplication].statusBarOrientation == UIDeviceOrientationPortraitUpsideDown));
                //
                //                glUniform4i(uniform.nLocation, landRight, landLeft, portrait, upsideDown);
                
                LOG_INFO("pfxDEVICEORIENTATION not implemented\n");
                break;
            }
                
            case pfxNODEINDEX:
            {
                // perform the inverse of readPixels on nodeIndex, so that when reading the Pixels, we can get the nodeIndex
                //glUniform4f(uniform.nLocation, (index & 0xFF)/255.0, ((index >> 8) & 0xFF)/255.0, ((index >> 16) & 0xFF)/255.0, ((index >> 24) & 0xFF)/255.0);
                //NSLog(@"pfxNODEINDEX = %d\n", index);
                glUniform4f(uniform.nLocation, index/255.0f, index/255.0f, index/255.0f, 1.0f);
                break;
            }
            case pfxIS_HIGHLIGHTED:
            {
                if(highlight)
                    glUniform1i(uniform.nLocation, 1);
                else
                    glUniform1i(uniform.nLocation, 0);
                
                break;
            }
            case pfxIS_CLEAR_LAYER:
            {
                if(drawPass == 2 /*|| pass == 1*/)
                    glUniform1i(uniform.nLocation, 1);
                else
                    glUniform1i(uniform.nLocation, 0);
                
                break;
            }
            case pfxPICKER_INFO:
            {
                //                glUniform4i(uniform.nLocation, self.pickingMode, scene.currentPickedIndex, index - scene.pickingIndexOffset, scene.numPickingNodes);
                glUniform4i(uniform.nLocation, 0, 1, 2, 3);
                break;
            }
            case pfxMULTIPLIED_COLOR:
            {
                glUniform3f(uniform.nLocation, 0, 0, 0);
                LOG_INFO("pfxMULTIPLIED_COLOR not implemented\n");
                break;
            }
            default:
                LOG_INFO("Semantic not supported:%d\n", uniform.nSemantic);
                break;
        }
    }
}

void Orca3dEffects::disableVertexAttribArray()
{
    // Safely disable the vertex attribute arrays
    glDisableVertexAttribArray(ePVRTPFX_UsPOSITION);
    glDisableVertexAttribArray(ePVRTPFX_UsNORMAL);
    glDisableVertexAttribArray(ePVRTPFX_UsTANGENT);
    glDisableVertexAttribArray(ePVRTPFX_UsBINORMAL);
    glDisableVertexAttribArray(ePVRTPFX_UsUV);
    glDisableVertexAttribArray(ePVRTPFX_UsVERTEXCOLOR);
    glDisableVertexAttribArray(ePVRTPFX_UsBONEINDEX);
    glDisableVertexAttribArray(ePVRTPFX_UsBONEWEIGHT);
}