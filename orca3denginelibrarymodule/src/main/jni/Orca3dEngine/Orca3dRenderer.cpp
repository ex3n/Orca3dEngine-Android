//
//  Orca3dRenderer.cpp
//  Orca3D
//
//  Created by Gary Robinson on 12/3/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#include "Orca3dRenderer.h"
#include "logger.h"

#define LOG_TAG "layer"

Orca3dRenderer::Orca3dRenderer(Orca3dScene *pScene)
{
    m_pScene = pScene;
    m_pEffects = new Orca3dEffects();
    
    m_pVbo = NULL;
    m_pIndexVbo = NULL;
    m_nNumMesh = 0;
    
    m_nNumDrawPasses = 1;
    m_bAlphaBlending = false;
    m_bTransparentSkinMode = false;
    
    m_fAnimationProgress = 0.f;
}

Orca3dRenderer::~Orca3dRenderer()
{
    if(m_pEffects)
    {
        delete m_pEffects;
        m_pEffects = NULL;
    }
    
    if(m_pVbo)
    {
        glDeleteBuffers(m_nNumMesh, m_pVbo);
        glDeleteBuffers(m_nNumMesh, m_pIndexVbo);
        
        delete []m_pVbo;
        delete []m_pIndexVbo;
    }
}

void Orca3dRenderer::setEffectsFile(const char *filename)
{
    m_pEffects->setEffectsFile(filename);
}

void Orca3dRenderer::initMeshData()
{
    m_nNumMesh = m_pScene->getModelPOD()->nNumMesh;
    
    if (!m_pVbo)
		m_pVbo = new GLuint[m_nNumMesh];
    
	if (!m_pIndexVbo)
		m_pIndexVbo = new GLuint[m_nNumMesh];
    
	glGenBuffers(m_nNumMesh, m_pVbo);
	for (unsigned int i = 0; i < m_nNumMesh; ++i)
	{
		// Load vertex data into buffer object
		SPODMesh &Mesh = m_pScene->getModelPOD()->pMesh[i];
		unsigned int uiSize = Mesh.nNumVertex * Mesh.sVertex.nStride;
        
		glBindBuffer(GL_ARRAY_BUFFER, m_pVbo[i]);
		glBufferData(GL_ARRAY_BUFFER, uiSize, Mesh.pInterleaved, GL_STATIC_DRAW);
        
		// Load index data into buffer object if available
		m_pIndexVbo[i] = 0;
		if (Mesh.sFaces.pData)
		{
			glGenBuffers(1, &m_pIndexVbo[i]);
			uiSize = PVRTModelPODCountIndices(Mesh) * sizeof(GLshort);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pIndexVbo[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, uiSize, Mesh.sFaces.pData, GL_STATIC_DRAW);
		}
	}
    
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Orca3dRenderer::enableAlphaBlend(bool enabled, bool transparentSkinMode)
{
    m_bAlphaBlending = enabled;
    m_bTransparentSkinMode = transparentSkinMode;
    
    if(!m_bAlphaBlending)
        m_nNumDrawPasses = 1;
    else if(!m_bTransparentSkinMode)
        m_nNumDrawPasses = 2;
    else
        m_nNumDrawPasses = 3;
}

void Orca3dRenderer::renderFrame(float timeSinceLastFrame)
{
    //LOG_INFO("started Render Frame");
    updateFrameRate();
    
    m_pScene->processTimeAnimations(timeSinceLastFrame);
    m_fAnimationProgress = m_pScene->getActiveAnimationProgress();
    
    m_pScene->setMatricesForCurrentFrame();
    
    m_pScene->updateLabelsForCurrentFrame();
    
    for(int drawPass = 0; drawPass < m_nNumDrawPasses; drawPass++)
    {
        if(initDrawPass(drawPass))

            drawVisibleLayers(drawPass);
    }
}

void Orca3dRenderer::updateFrameRate()
{
    // do this later
}

float Orca3dRenderer::getFrameRate()
{
    return m_fFrameRate;
}

bool Orca3dRenderer::initDrawPass(int passIndex)
{
    if(passIndex == 0)
    {
       glDepthMask(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		//LOG_INFO("glclear after gldepth mask");
    }
    else
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    }
    return true;
}

void Orca3dRenderer::drawVisibleLayers(int passIndex)
{
    for(int i=0; i<m_pScene->getNumLayers(); i++)
    {
        Orca3dLayer *layer = m_pScene->getLayerAt(i);

        if(layer->getVisible())
        {
         //LOG_INFO("LayerName %s " , layer->getName().c_str() );
            if(passIndex == 0)
            {
                //regular nodes
                for(int j=0; j<layer->getNumNodeIndices(); j++)
                {
                    int nodeIndex = layer->getNodeIndexAt(j);
                    drawNode(nodeIndex, passIndex, layer->getHighlighted());
                }
                
                //labels
                if(layer->getLabelsVisible())
                {
                    for(int k=0; k<layer->getNumLabelIndices(); k++)
                    {
                        int labelNodeIndex = layer->getLabelIndexAt(k);
                        drawNode(labelNodeIndex, passIndex, layer->getHighlighted());
                    }
                }
            }
            else
            {
                //alpha blended nodes
                for(int k=0; k<layer->getNumAlphaIndices(); k++)
                {
                    int alphaNodeIndex = layer->getAlphaIndexAt(k);
                    drawNode(alphaNodeIndex, passIndex, layer->getHighlighted());
                }
            }
            
        }
    }
}

void Orca3dRenderer::drawNode(int nodeIndex, int drawPassIndex, bool bIsHighlighted)
{
    SPODNode &node = m_pScene->getModelPOD()->pNode[nodeIndex];
    
    setCullingForNode(nodeIndex, drawPassIndex);
    
    bool bIsLabel = false;
    if(drawPassIndex == 0)
    {
        string nodeName(node.pszName);
        if(nodeName.find("annotation") != string::npos)
            bIsLabel = true;
        
        //LOG_INFO("drawing %s\n", nodeName.c_str());
    }
    
    // bind the VBO for the mesh
    GLuint vbo = m_pVbo[node.nIdx];
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // bind the index buffer, won't hurt if the handle is 0
    GLuint vboIndex = m_pIndexVbo[node.nIdx];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndex);

    //LOG_INFO("processing Effects");
    //set up the shaders for the node
    m_pEffects->processEffects(m_pScene, nodeIndex, drawPassIndex, bIsHighlighted, m_fAnimationProgress, bIsLabel);
    //LOG_INFO("done processing Effects");

    SPODMesh &mesh = m_pScene->getModelPOD()->pMesh[node.nIdx];
    int boneCount = mesh.sBoneIdx.n;
    if(boneCount > 0)
        drawBones(nodeIndex, drawPassIndex, bIsLabel);
    else
        glDrawElements(GL_TRIANGLES, mesh.nNumFaces * 3, GL_UNSIGNED_SHORT, &((unsigned short*)0)[0]);

    //LOG_INFO("glDrawElements worked");
    // Unbind the previous buffers by binding to 0
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    m_pEffects->disableVertexAttribArray();
}

void Orca3dRenderer::setCullingForNode(int nodeIndex, int passIndex)
{
    glEnable(GL_CULL_FACE);
    if(m_bTransparentSkinMode && (passIndex == 1))
    {
        SPODNode &node = m_pScene->getModelPOD()->pNode[nodeIndex];
        string nodeName(node.pszName);
        if((nodeName.find("skin") != string::npos) || (nodeName.find("Skin") != string::npos))
        {
            glCullFace(GL_FRONT);
            return;
        }
    }
    glCullFace(GL_BACK);
}

void Orca3dRenderer::drawBones(int nodeIndex, int drawPassIndex, bool bIsLabel)
{
    SPODNode &node = m_pScene->getModelPOD()->pNode[nodeIndex];
    SPODMesh &mesh = m_pScene->getModelPOD()->pMesh[node.nIdx];
    
    for(int i32Batch = 0; i32Batch < mesh.sBoneBatches.nBatchCnt; ++i32Batch)
    {
        /*
         If the current mesh has bone index and weight data then we need to
         set up some additional variables in the shaders.
         */
        
        // Go through the bones for the current bone batch
        PVRTMat4 amBoneWorld[16];
        PVRTMat3 afBoneWorldIT[16], mBoneIT;
        
        int i32Count = mesh.sBoneBatches.pnBatchBoneCnt[i32Batch];
        
        if(i32Count > 16){
            printf("ERROR:  Hard code matrix aray is too short!!!!!");
        }
        
        for(int i = 0; i < i32Count; ++i)
        {
            // Get the Node of the bone
            int i32NodeID = mesh.sBoneBatches.pnBatches[i32Batch * mesh.sBoneBatches.nBatchBoneMax + i];
            
            // Get the World transformation matrix for this bone and combine it with our app defined
            // transformation matrix
            amBoneWorld[i] = m_pScene->getModelPOD()->GetBoneWorldMatrix(node, m_pScene->getModelPOD()->pNode[i32NodeID]);
            
            //on the pin annotations with bones in them, use boneWorldMatrix, not curNodeWorldMatrix
            if(bIsLabel)
            {
                amBoneWorld[i] *= m_pScene->getLabelScaleMatrix();
                glUniformMatrix4fv(m_pEffects->getCurNodeWorldMatrixLoc(), 1, GL_FALSE, m_pScene->getProjectionViewRot().ptr());
            }
            
            // Calculate the inverse transpose of the 3x3 rotation/scale part for correct lighting
            afBoneWorldIT[i] = PVRTMat3(amBoneWorld[i]).inverse().transpose();
        }
        
        if(m_pEffects->curNodeHasBoneMatLoc())
            glUniformMatrix4fv(m_pEffects->getCurNodeBoneMatLoc(), i32Count, GL_FALSE, amBoneWorld[0].ptr());
        
        if(m_pEffects->curNodeHasBoneMatITLoc())
            glUniformMatrix3fv(m_pEffects->getCurNodeBoneMatITLoc(), i32Count, GL_FALSE, afBoneWorldIT[0].ptr());
        
        /*
         As we are using bone batching we don't want to draw all the faces contained within pMesh, we only want
         to draw the ones that are in the current batch. To do this we pass to the drawMesh function the offset
         to the start of the current batch of triangles (Mesh.sBoneBatches.pnBatchOffset[i32Batch]) and the
         total number of triangles to draw (i32Tris)
         */
        int i32Tris;
        if(i32Batch+1 < mesh.sBoneBatches.nBatchCnt)
            i32Tris = mesh.sBoneBatches.pnBatchOffset[i32Batch+1] - mesh.sBoneBatches.pnBatchOffset[i32Batch];
        else
            i32Tris = mesh.nNumFaces - mesh.sBoneBatches.pnBatchOffset[i32Batch];
        
        // Draw the mesh one bone batch at a time
        glDrawElements(GL_TRIANGLES, i32Tris * 3, GL_UNSIGNED_SHORT, &((unsigned short*)0)[3 * mesh.sBoneBatches.pnBatchOffset[i32Batch]]);
    }
}

void Orca3dRenderer::pickLabelAtViewPosition(float viewX, float viewY)
{
    //render a frame offscreen with picking flag set for the effects
    m_pEffects->setPicking(true);
    
    //turn off the transparent skin layer from any renderings so labels can be picked through it
    bool bTurnOffSkin = false;
    const char *skinLayerName = "Skin";
    if(m_bTransparentSkinMode && m_pScene->isLayerVisible(skinLayerName))
    {
        m_pScene->setLayerVisible(skinLayerName, false);
        bTurnOffSkin = true;
    }
    
    //?? scale the pins bigger so they are easier to touch
    
    GLuint viewFrameBuffer, viewRenderBuffer, depthRenderbuffer;
    glGenFramebuffers(1, &viewFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, viewFrameBuffer);
    
    
    int bufferWidth = (int)(m_pScene->getWidth());
    int bufferHeight = (int)(m_pScene->getHeight());
    glGenRenderbuffers(1, &viewRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, bufferWidth, bufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, viewRenderBuffer);
    
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, bufferWidth, bufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    glBindFramebuffer(GL_FRAMEBUFFER, viewFrameBuffer);
    glViewport(0, 0, bufferWidth, bufferHeight);
    
    renderFrame(0.f);
    
    // get the color at the pixel touched, use that to determine where the model was picked
    unsigned char pixels[4] = {0,0,0,0};
	glBindFramebuffer(GL_FRAMEBUFFER, viewFrameBuffer);
    int readPixelsX = (int)(viewX);
    int readPixelsY = (int)(m_pScene->getHeight() - viewY);
	glReadPixels(readPixelsX, readPixelsY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    //if the pixel touched is not the clear color, then a node was picked
    int nodeIndex = -1;
    float clearColor[4] = {-1,-1,-1,-1};
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    for(int i=0; i<4; i++)
    {
        if(clearColor[i] != pixels[i])
        {
            nodeIndex = (int)(pixels[0]);
            break;
        }
    }
    m_pScene->pickedNode(nodeIndex);
    
    //cleanup
    glDeleteFramebuffers(1, &viewFrameBuffer);
    glDeleteRenderbuffers(1, &viewRenderBuffer);
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    
    m_pEffects->setPicking(false);
    
    if(bTurnOffSkin)
        m_pScene->setLayerVisible(skinLayerName, true);
    
}

GLubyte *Orca3dRenderer::renderToImageData(int bufferWidth, int bufferHeight, int dataLength)
{
    GLuint viewFrameBuffer, viewRenderBuffer, depthRenderbuffer;
    glGenFramebuffers(1, &viewFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, viewFrameBuffer);
    
    glGenRenderbuffers(1, &viewRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, bufferWidth, bufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, viewRenderBuffer);
    
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, bufferWidth, bufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    glBindFramebuffer(GL_FRAMEBUFFER, viewFrameBuffer);
    glViewport(0, 0, bufferWidth, bufferHeight);
    
    renderFrame(0);
    
    GLubyte *data = (GLubyte *)malloc(dataLength * sizeof(GLubyte));
    
    glBindFramebuffer(GL_FRAMEBUFFER, viewFrameBuffer);
    glReadPixels(0, 0, bufferWidth, bufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    return data;
}
