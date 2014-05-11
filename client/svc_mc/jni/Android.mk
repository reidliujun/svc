LOCAL_PATH := $(call my-dir)
-include android-ndk-profiler.mk
include $(CLEAR_VARS)




LOCAL_MODULE    += svc

APPDIR := $(shell readlink $(LOCAL_PATH)/src)
APP_SUBDIRS := $(patsubst $(LOCAL_PATH)/%, %, $(shell find $(LOCAL_PATH)/$(APPDIR) -path '*/.svn' -prune -o -type d -print))

LOCAL_SRC_FILES += $(foreach F, $(APP_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.cpp))))
LOCAL_SRC_FILES += $(foreach F, $(APP_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.c))))
#LOCAL_SRC_FILES += $(foreach F, $(APP_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.s))))
#uses-permission android:name="android.permission.ACCESS_SURFACE_FLINGER"
#ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
#    LOCAL_CFLAGS += -DHAVE_NEON=1
#    LOCAL_SRC_FILES += SVC/neonpart.c.neon
#endif

LOCAL_CFLAGS += -O3 \
				$(foreach D, $(APP_SUBDIRS), -I$(LOCAL_PATH)/$(D)) 
				
#LOCAL_CFLAGS += -pg -DPOCKET_PC -DTI_OPTIM
LOCAL_CFLAGS += -march=armv7-a -mtune=cortex-a8 -mfpu=neon -O3 -fpic -mthumb-interwork -mfloat-abi=softfp -ftree-vectorize -fomit-frame-pointer -ffast-math -funroll-loops -funsafe-math-optimizations -fsingle-precision-constant 
LOCAL_STATIC_LIBRARIES := andprof cpufeatures
LOCAL_LDLIBS    := -lm -llog -ljnigraphics
#LOCAL_LDLIBS 	+= -g
LOCAL_CFLAGS += -g

include $(BUILD_SHARED_LIBRARY) 

$(call import-module,cpufeatures)
