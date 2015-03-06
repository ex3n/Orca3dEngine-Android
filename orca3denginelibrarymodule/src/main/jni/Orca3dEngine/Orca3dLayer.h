//
//  Orca3dLayers.h
//  Orca3D
//
//  Created by Gary Robinson on 12/3/13.
//  Copyright (c) 2013 Chad Zeluff. All rights reserved.
//

#ifndef __Orca3D__Orca3dLayers__
#define __Orca3D__Orca3dLayers__

#include <iostream>
#include <vector>
#include <string>
#include "OGLES2Tools.h"
using namespace std;

class Orca3dLayer
{
public:
    Orca3dLayer();
    ~Orca3dLayer();
    
    void setName(string name);
    string getName();
    
    void setVisible(bool visible);
    bool getVisible();
    
    void setLabelsVisible(bool visible);
    bool getLabelsVisible();
    
    void setHighlighted(bool highlight);
    bool getHighlighted();
    
    void addNodeNameSubstring(string substring);
    void excludeNodeNameSubstring(string substring);
    void addAlphaNodeNameSubstring(string substring);
    void generateNodeIndices(CPVRTModelPOD *pModelPOD);
    
    int getNumNodeIndices();
    int getNodeIndexAt(int index);
    
    int getNumLabelIndices();
    int getLabelIndexAt(int index);
    
    int getNumAlphaIndices();
    int getAlphaIndexAt(int index);
    
private:
    string m_sName;
    bool m_bVisible;
    bool m_bLabelsVisible;
    bool m_bHighlighted;
    vector<string> m_vNodeNameSubstrings;
    vector<string> m_vExcludeNodeNameSubstrings;
    vector<string> m_vAlphaNodeNameSubstrings;
    vector<int> m_vNodeIndices;
    vector<int> m_vLabelNodeIndices;
    vector<int> m_vAlphaNodeIndices;
};

#endif /* defined(__Orca3D__Orca3dLayers__) */