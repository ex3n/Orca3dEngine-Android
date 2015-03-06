LOCAL_PATH := $(realpath $(call my-dir)/../../.. )

PVRSDKDIR := $(LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE    := ogles2tools

### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES :=      OGLES2/PVRTBackground.cpp \
			OGLES2/PVRTgles2Ext.cpp \
			OGLES2/PVRTPFXParserAPI.cpp \
			OGLES2/PVRTPrint3DAPI.cpp \
			OGLES2/PVRTPFXSemantics.cpp \
			OGLES2/PVRTShader.cpp \
			OGLES2/PVRTTextureAPI.cpp

LOCAL_SRC_FILES +=      PVRTDecompress.cpp \
			PVRTBoneBatch.cpp \
			PVRTError.cpp \
			PVRTFixedPoint.cpp \
			PVRTGeometry.cpp \
			PVRTMatrixF.cpp \
			PVRTMatrixX.cpp \
			PVRTMisc.cpp \
			PVRTModelPOD.cpp \
			PVRTPFXParser.cpp \
			PVRTPrint3D.cpp \
			PVRTQuaternionF.cpp \
			PVRTQuaternionX.cpp \
			PVRTResourceFile.cpp \
			PVRTShadowVol.cpp \
			PVRTString.cpp \
			PVRTStringHash.cpp \
			PVRTTexture.cpp \
		        PVRTTrans.cpp \
		        PVRTTriStrip.cpp \
			PVRTUnicode.cpp \
			PVRTVector.cpp \
			PVRTVertex.cpp

LOCAL_C_INCLUDES := $(PVRSDKDIR)/OGLES2 $(PVRSDKDIR) $(PVRSDKDIR)/OGLES2/Include

LOCAL_CFLAGS := -DBUILD_OGLES2 -DGL2JNI

ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_CFLAGS += -fno-stack-protector 
endif

include $(BUILD_STATIC_LIBRARY)
