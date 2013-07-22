LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_MODULE := asmlibrary
LOCAL_SRC_FILES := so/$(TARGET_ARCH_ABI)/libasmlibrary.so
include $(PREBUILT_SHARED_LIBRARY) 


include $(CLEAR_VARS)

#OPENCV_CAMERA_MODULES:=off
#OPENCV_INSTALL_MODULES:=off
#OPENCV_LIB_TYPE:=SHARED
include /media/work/Develop/opencv/OpenCV-2.4.6-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES  := DemoFit.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_CFLAGS    += -DOPENCV_OLDER_VISION
LOCAL_LDLIBS     += -llog -ldl  

LOCAL_MODULE     := jni-asmlibrary

LOCAL_SHARED_LIBRARIES := asmlibrary

include $(BUILD_SHARED_LIBRARY)
