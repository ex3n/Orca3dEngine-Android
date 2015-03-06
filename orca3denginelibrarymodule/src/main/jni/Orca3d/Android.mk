# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


LOCAL_PATH := $(realpath $(call my-dir)/..)
PVRSDKDIR := $(LOCAL_PATH)/Orca3dEngine/Libraries/PowerVR
LOCALDIR := $(LOCAL_PATH)/Heart
ORCA3DDIR := $(LOCAL_PATH)/Orca3dEngine
MEDIADIR := $(LOCALDIR)/Media
MODELDIR := $(LOCALDIR)/Content/Anatomy
ASSETDIR := $(LOCALDIR)/OGLES2/Build/Android/assets

### DO NOT USE $(LOCAL_PATH) AFTER THIS POINT

include $(CLEAR_VARS)

LOCAL_MODULE    := libOrca3DEngine

### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := \
                    OrcaNativeJavaAPI.cpp \
                    AndroidRenderer.cpp    \
				   Orca3dEngine/Libraries/PowerVR/Shell/PVRShell.cpp \
				   Orca3dEngine/Libraries/PowerVR/Shell/API/KEGL/PVRShellAPI.cpp \
				   Orca3dEngine/Libraries/PowerVR/Shell/OS/Android/PVRShellOS.cpp \
				   Orca3dEngine/Orca3dPVRShell.cpp  \
				   Orca3dEngine/Orca3dAndroid.cpp \
				   Orca3dEngine/Orca3dAnimation.cpp \
				   Orca3dEngine/Orca3dController.cpp \
				   Orca3dEngine/Orca3dEffects.cpp \
				   Orca3dEngine/Orca3dLayer.cpp \
				   Orca3dEngine/Orca3dRenderer.cpp \
				   Orca3dEngine/Orca3dScene.cpp
				   
LOCAL_C_INCLUDES :=	\
				    $(ORCA3DDIR) \
				    $(PVRSDKDIR)/Shell	\
				    $(PVRSDKDIR)/Shell/API/KEGL	\
				    $(PVRSDKDIR)/Shell/OS/Android	\
				    $(PVRSDKDIR)/Build	\
				    $(PVRSDKDIR)/Build/Include	\
				    $(PVRSDKDIR)/OGLES2/Include	\
				    $(PVRSDKDIR)	\
				    $(PVRSDKDIR)/OGLES2

LOCAL_CFLAGS := -DBUILD_OGLES2 -DGL2JNI

ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_CFLAGS += -fno-stack-protector
endif

LOCAL_LDLIBS :=  \
				-llog \
				-landroid \
				-lEGL \
				-lGLESv2

LOCAL_STATIC_LIBRARIES := \
				          android_native_app_glue \
				          ogles2tools 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

