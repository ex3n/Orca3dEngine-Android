//
//  Orca3dAnimation.h
//  Orca3D
//
//  Created by Gary Robinson on 12/10/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#ifndef __Orca3D__Orca3dAnimation__
#define __Orca3D__Orca3dAnimation__

#define PI        3.14159265358979323846264338327950288f

#include <iostream>
#include <vector>
#include <string>
#include "OGLES2Tools.h"
using namespace std;

class Orca3dAnimation
{
public:
    Orca3dAnimation();
    ~Orca3dAnimation();
    
    string getName();
    void setName(string name);
    
    void setFrames(int start, int end, bool oscillates);
    
    void activate(bool activate);
    bool isActive();
    
    void set2DBased(string type);
    void set2DProgress(float progress1, float progress2, CPVRTModelPOD *pModelPOD);
    
    void set1DBased();
    void set1DProgress(float progress);
    
    void setTimeBased(float duration, bool repeats);
    bool isTimeBased();
    bool advanceProgress(float duration);
    float getProgress();
    
    void applyCurrentFrameToModel(CPVRTModelPOD *pModelPOD);
    
private:
    string m_sName;
    bool m_bActivated;
    
    int m_nStartFrame;
    int m_nEndFrame;
    float m_fCurrentFrame;
    
    float m_fProgress;
    bool m_bOscillates;
    
    bool m_bTimeBased;
    float m_fDuration;
    bool m_bRepeats;
    
    bool m_b2DBased;
    string m_s2DType;
};

#endif /* defined(__Orca3D__Orca3dAnimation__) */