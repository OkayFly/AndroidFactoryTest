LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= factory_test_server.c data.c factory_test_function.c serial_test.c can_test.c
LOCAL_MODULE:= factory_test_server
LOCAL_32_BIT_ONLY := true
LOCAL_LDLIBS += -lc
#LOCAL_C_INCLUDES += system/core/include/utils
#LOCAL_SHARED_LIBRARIES := libcutil
include $(BUILD_EXECUTABLE)

#include $(BUILD_SHARD_LIBRARY)
