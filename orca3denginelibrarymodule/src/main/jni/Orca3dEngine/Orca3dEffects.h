//
//  Orca3dEffects.h
//  Orca3D
//
//  Created by Gary Robinson on 12/4/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#ifndef __Orca3D__Orca3dEffects__
#define __Orca3D__Orca3dEffects__

#include <iostream>
#include <vector>
#include <string>
#include "OGLES2Tools.h"
#include "Orca3dScene.h"
using namespace std;



class Orca3dSEffect
{
public:
    Orca3dSEffect();
    ~Orca3dSEffect();
    
    void init(CPVRTPFXParser *parser, const char *effectName, const char *filename);
    void initTextures(vector<string> *pTextureNames, vector<GLuint> *pTextureLocations);
    void activate();
    
    unsigned int getNumUniforms();
    SPVRTPFXUniform getUniformAt(unsigned int index);
    
private:
    void cleanup();
    
    CPVRTPFXEffect *m_pEffect;
    unsigned int m_ui32UniformCount;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

class Orca3dEffects
{
public:
    Orca3dEffects();
    ~Orca3dEffects();
    
    void setEffectsFile(const char * filename);
    void processEffects(Orca3dScene *pScene, int index, int drawPass, bool highlight, float progress, bool bIsLabel);
    void disableVertexAttribArray();
    
    bool curNodeHasBoneMatLoc();
    bool curNodeHasBoneMatITLoc();
    GLuint getCurNodeBoneMatLoc();
    GLuint getCurNodeBoneMatITLoc();
    GLuint getCurNodeWorldMatrixLoc();
    
    void setPicking(bool picking);
    
private:
    void closeEffectParser();
    bool loadEffect(char *effectName, char *filename);
    string removeExtension(string effectName);
    
    CPVRTPFXParser *m_pEffectParser;
    
    vector<string> m_vTextureNames;
    vector<GLuint> m_vTextureLocations;
    
    vector<string> m_vEffectNames;
    vector<Orca3dSEffect*> m_vSEffects;
    unsigned int m_nNoTexturesIndex;
    unsigned int m_nPickingIndex;
    
    bool m_curNodeHasBoneMatLoc;
    bool m_curNodeHasBoneMatITLoc;
    GLuint m_boneMatrixArrayLoc;
    GLuint m_boneMatrixArrayITLoc;
    GLuint m_boneWorldMatrixLoc;
    
    bool m_bPicking;
};


#endif /* defined(__Orca3D__Orca3dEffects__) */