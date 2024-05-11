#include <jni.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_mercury_testbed_MainActivity_stringFromJNI(
	JNIEnv* env,
	jobject thiz) {
	return env->NewStringUTF("Hello Sashulya");
}


