LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := klotski
LOCAL_SRC_FILES := HashTable.c Klotski.c zb_zxy_zyc_klotski_Game.c
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)