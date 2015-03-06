ifeq ($(shell uname),Darwin)
TEXTOOL_PATH  =  $(PVRSDKDIR)/Utilities/PVRTexTool/PVRTexToolCL/MacOS_x86/PVRTexTool
FILEWRAP_PATH =  $(PVRSDKDIR)/Utilities/Filewrap/MacOS_x86_32/Filewrap
UNISCO_PATH   =  $(PVRSDKDIR)/Utilities/PVRUniSCo/OGLES/MacOS_x86_32/glslcompiler_sgx535
else 
ifeq ($(shell uname -o),Cygwin)
TEXTOOL_PATH  =  $(shell cygpath -awm $(PVRSDKDIR)/Utilities/PVRTexTool/PVRTexToolCL/Windows_x86_32/PVRTexTool)
FILEWRAP_PATH =  $(shell cygpath -awm $(PVRSDKDIR)/Utilities/Filewrap/Windows_x86_32/Filewrap)
UNISCO_PATH   =  $(shell cygpath -awm $(PVRSDKDIR)/Utilities/PVRUniSCo/OGLES/Windows_x86_32/glslcompiler_sgx535)
else
TEXTOOL_PATH  =  $(PVRSDKDIR)/Utilities/PVRTexTool/PVRTexToolCL/Linux_x86_32/PVRTexTool
FILEWRAP_PATH =  $(PVRSDKDIR)/Utilities/Filewrap/Linux_x86_32/Filewrap
UNISCO_PATH   =  $(PVRSDKDIR)/Utilities/PVRUniSCo/OGLES/Linux_x86_32/glslcompiler_sgx535
endif
endif