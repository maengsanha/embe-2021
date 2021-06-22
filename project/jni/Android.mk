LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ndk-exam
# LOCAL_MODULE_TAGS := optional eng
# LOCAL_PRELINK_MODULE := false
LOCAL_SRC_FILES := first.c second.c
LOCAL_LDLIBS := -llog
# LOCAL_LDLIB := -L$(SYSROOT)/usr/lib -llog
# LOCAL_LD_FLAGS += $(TARGET_OUT_INTERMEDIATE_LIBRARIES)/liblog.so

include $(BUILD_SHARED_LIBRARY)
