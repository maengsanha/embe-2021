#include <jni.h>
#include "android/log.h"

#define LOG_TAG "MyTag"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

extern char *get_process_info();

jstring JNICALL Java_org_example_ndk_Monitor_getProcessInfo(JNIEnv *env, jobject this) {
  char *buf = get_process_info();
  return buf;
}
