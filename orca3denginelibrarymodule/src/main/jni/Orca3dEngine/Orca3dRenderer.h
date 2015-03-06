//
//  Orca3dRenderer.h
//  Orca3D
//
//  Created by Gary Robinson on 12/3/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#ifndef __Orca3D__Orca3dRenderer__
#define __Orca3D__Orca3dRenderer__

#include <iostream>
#include "OGLES2Tools.h"
#include "Orca3dScene.h"
#include "Orca3dEffects.h"
//using namespace std;

class Orca3dRenderer
{
public:
    Orca3dRenderer(Orca3dScene *pScene);
    ~Orca3dRenderer();
    
    void setEffectsFile(const char *filename);
    void initMeshData();
    
    void enableAlphaBlend(bool enabled, bool transparentSkinMode);
    
    void renderFrame(float timeSinceLastFrame);
    float getFrameRate();
    
    void pickLabelAtViewPosition(float viewX, float viewY);
    
    GLubyte *renderToImageData(int bufferWidth, int bufferHeight, int dataLength);
    
private:
    void updateFrameRate();
    bool initDrawPass(int passIndex);
    void drawVisibleLayers(int passIndex);
    void drawNode(int nodeIndex, int drawPassIndex, bool bIsHighlighted);
    void setCullingForNode(int nodeIndex, int passIndex);
    void drawBones(int nodeIndex, int drawPassIndex, bool bIsLabel);
    
    Orca3dScene *m_pScene;
    Orca3dEffects *m_pEffects;
    
    int m_nNumDrawPasses;
    float m_fFrameRate;
    
    float m_fAnimationProgress;
    
    //vertex and index buffers
    GLuint* m_pVbo;
	GLuint* m_pIndexVbo;
    unsigned int m_nNumMesh;
    
    //alpha blending
    bool m_bAlphaBlending;
    bool m_bTransparentSkinMode;
    
    //needed for a rendering pass - this data should probably be in the render controller then
    PVRTMat4 m_pinScaleMatrix;
    
    
};

#endif /* defined(__Orca3D__Orca3dRenderer__) */