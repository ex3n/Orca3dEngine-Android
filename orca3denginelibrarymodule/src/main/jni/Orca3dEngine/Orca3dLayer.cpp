//
//  Orca3dLayers.cpp
//  Orca3D
//
//  Created by Gary Robinson on 12/3/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#include "Orca3dLayer.h"

Orca3dLayer::Orca3dLayer()
{
    m_sName = "";
    m_bVisible = false;
    m_bLabelsVisible = false;
    m_bHighlighted = false;
}

Orca3dLayer::~Orca3dLayer()
{
    
}

void Orca3dLayer::setName(string name)
{
    m_sName = name;
}
string Orca3dLayer::getName()
{
    return m_sName;
}

void Orca3dLayer::setVisible(bool visible)
{
    m_bVisible = visible;
}
bool Orca3dLayer::getVisible()
{
    return m_bVisible;
}

void Orca3dLayer::setLabelsVisible(bool visible)
{
    m_bLabelsVisible = visible;
}
bool Orca3dLayer::getLabelsVisible()
{
    return m_bLabelsVisible;
}

void Orca3dLayer::setHighlighted(bool highlight)
{
    m_bHighlighted = highlight;
}
bool Orca3dLayer::getHighlighted()
{
    return m_bHighlighted;
}

void Orca3dLayer::addNodeNameSubstring(string substring)
{
    m_vNodeNameSubstrings.push_back(substring);
}
void Orca3dLayer::excludeNodeNameSubstring(string substring)
{
    m_vExcludeNodeNameSubstrings.push_back(substring);
}
void Orca3dLayer::addAlphaNodeNameSubstring(string substring)
{
    m_vAlphaNodeNameSubstrings.push_back(substring);
}
void Orca3dLayer::generateNodeIndices(CPVRTModelPOD *pModelPOD)
{
    bool bLogLayers = false;
    //bLogLayers = true;
    
    m_vNodeIndices.clear();
    m_vLabelNodeIndices.clear();
    m_vAlphaNodeIndices.clear();
    
    for(unsigned int i=0; i<pModelPOD->nNumMeshNode; i++)
    {
        SPODNode& Node = pModelPOD->pNode[i];
        string nodeName(Node.pszName);
        
        bool bFound = false;
		for (unsigned int j = 0; j<m_vNodeNameSubstrings.size(); j++)
        {
            if(nodeName.find(m_vNodeNameSubstrings.at(j)) != string::npos)
            {
                bFound = true;
                break;
            }
        }
        
        if(bFound)
        {
            //make sure it isn't explicitly excluded from the layer
			for (unsigned int j = 0; j<m_vExcludeNodeNameSubstrings.size(); j++)
            {
                if(nodeName.find(m_vExcludeNodeNameSubstrings.at(j)) != string::npos)
                {
                    bFound = false;
                    break;
                }
            }
        }
        
        if(bFound)
        {
            //add it as a label, alpha, or regular node
            if(nodeName.find("annotation") != string::npos)
            {
                if(bLogLayers)
                    printf("added %s to labels for layer %s\n", nodeName.c_str(), m_sName.c_str());
                m_vLabelNodeIndices.push_back(i);
            }
            else
            {
                bool bAlpha = false;
				for (unsigned int j = 0; j<m_vAlphaNodeNameSubstrings.size(); j++)
                {
                    if(nodeName.find(m_vAlphaNodeNameSubstrings.at(j)) != string::npos)
                    {
                        if(bLogLayers)
                            printf("added %s to alpha for layer %s\n", nodeName.c_str(), m_sName.c_str());
                        m_vAlphaNodeIndices.push_back(i);
                        bAlpha = true;
                        break;
                    }
                }
                if(!bAlpha)
                {
                    if(bLogLayers)
                        printf("added %s to layer %s\n", nodeName.c_str(), m_sName.c_str());
                    m_vNodeIndices.push_back(i);
                }
            }
            
        }
    }
}

int Orca3dLayer::getNumNodeIndices()
{
    return (int)(m_vNodeIndices.size());
}
int Orca3dLayer::getNodeIndexAt(int index)
{
    return m_vNodeIndices.at(index);
}

int Orca3dLayer::getNumLabelIndices()
{
    return (int)(m_vLabelNodeIndices.size());
}
int Orca3dLayer::getLabelIndexAt(int index)
{
    return m_vLabelNodeIndices.at(index);
}

int Orca3dLayer::getNumAlphaIndices()
{
    return (int)(m_vAlphaNodeIndices.size());
}
int Orca3dLayer::getAlphaIndexAt(int index)
{
    return m_vAlphaNodeIndices.at(index);
}