//
//  Orca3dAnimation.cpp
//  Orca3D
//
//  Created by Gary Robinson on 12/10/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#include "Orca3dAnimation.h"

Orca3dAnimation::Orca3dAnimation()
{
    m_sName = "";
    m_nStartFrame = 0;
    m_nEndFrame = 0;
    m_fCurrentFrame = 0;
    m_bRepeats = false;
    m_bOscillates = false;
    m_bActivated = false;
    m_b2DBased = false;
    m_bTimeBased = false;
//    m_fProgress = 0;
}

Orca3dAnimation::~Orca3dAnimation()
{
    
}

string Orca3dAnimation::getName()
{
    return m_sName;
}

void Orca3dAnimation::setName(string name)
{
    m_sName = name;
}
void Orca3dAnimation::setFrames(int start, int end, bool oscillates)
{
    m_nStartFrame = start;
    m_nEndFrame = end;
    m_fCurrentFrame = (float)(start);
    m_bOscillates = oscillates;
    m_fProgress = 0.;
}

void Orca3dAnimation::activate(bool activate)
{
    m_bActivated = activate;
	//m_fProgress = 0.;
}
bool Orca3dAnimation::isActive()
{
    return m_bActivated;
}


void Orca3dAnimation::set2DBased(string type)
{
    m_b2DBased = true;
    m_s2DType = type;
}
void Orca3dAnimation::set2DProgress(float progress1, float progress2, CPVRTModelPOD *pModelPOD)
{
    if(m_s2DType.compare("eye") == 0)
    {
        int frame2 = (int)(progress2 * 20.0f);
        m_fCurrentFrame = (progress1 * 20.0f + frame2 * 20.0f);
        pModelPOD->SetFrame(m_fCurrentFrame);
    }
    else if(m_s2DType.compare("spine") == 0)
    {
        if(progress1 == 0.5 && progress2 == 0.5)
        {
            m_fCurrentFrame = 0;
        }
        else
        {
            //convert it into basically polar coordinates
            progress1 -= 0.5;
            progress2 -= 0.5;
            
            float angle = atan2f(-1.0f * progress2, progress1);
            angle -= PI/4.0f;
            if(angle < 0.)
                angle += 2.0f * PI;
            float angleFactor = (float)((int)(6.0f * 15.999f * angle * 0.5f / PI));
            
            float dist = sqrtf(progress1 * progress1 + progress2 * progress2);
            float distFactor = (float)((int)(6.0f * 2.999f * dist / sqrtf(2.0f)));
            
            m_fCurrentFrame = (angleFactor + 6.0f * 16.0f * distFactor);
        }
        pModelPOD->SetFrame(m_fCurrentFrame);
    }
}

void Orca3dAnimation::set1DBased()
{
    m_b2DBased = false;
}
void Orca3dAnimation::set1DProgress(float progress)
{
    if(progress > 1.0)
        progress = 1.0;
    if(progress < 0.)
        progress = 0.;
    
    m_fProgress = progress;
    
    if(m_bOscillates)
    {
        if(progress > 0.5f)
            progress = 2.0f * (1.0f - progress);
        else
            progress = 2.0f * progress;
    }
    
    m_fCurrentFrame = (float)(m_nStartFrame) + progress * (float)(m_nEndFrame - m_nStartFrame);
}

void Orca3dAnimation::setTimeBased(float duration, bool repeats)
{
    m_fDuration = duration;
    m_bRepeats = repeats;
    m_b2DBased = false;
    m_bTimeBased = true;
}

bool Orca3dAnimation::isTimeBased()
{
    return m_bTimeBased;
}

bool Orca3dAnimation::advanceProgress(float duration)
{
    if(m_fProgress >= 1.0)
    {
        if(m_bRepeats)
        {
            m_fProgress = 0.;
            return true;
        }
        else
            return false;
    }
    
    set1DProgress(m_fProgress + duration / m_fDuration);
    return true;
}

float Orca3dAnimation::getProgress()
{
    return m_fProgress;
}

void Orca3dAnimation::applyCurrentFrameToModel(CPVRTModelPOD *pModelPOD)
{
	if (!m_b2DBased)
		pModelPOD->SetFrame(m_fCurrentFrame);
}