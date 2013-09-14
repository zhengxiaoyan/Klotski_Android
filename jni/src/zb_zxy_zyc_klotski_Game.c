#include <stdlib.h>
#include "zb_zxy_zyc_klotski_Game.h"
#include "Klotski.h"

JNIEXPORT jobjectArray JNICALL Java_zb_zxy_zyc_klotski_Game_getStartBoards
  (JNIEnv *env, jobject obj)
{
	int i;
	jclass cls = NULL;
	jmethodID mid = 0;
	jobject board = NULL;
	jfieldID field_cells = NULL;
	jbyteArray cells = NULL;

	jobjectArray ret = NULL;
	StartBoard **sb = NULL;
	int size = 0;

	sb = (StartBoard **)malloc(sizeof(StartBoard *));
	size = getStartBoards(sb);
	DEBUG("start board array has size of %d", size);

	cls = (*env)->FindClass(env, "zb/zxy/zyc/klotski/Board");

	ret = (*env)->NewObjectArray(env, size, cls, NULL);
	for (i=0;i<size;i++) {
		DEBUG("set start board array at %d", i);
		cls = (*env)->FindClass(env, "zb/zxy/zyc/klotski/Board");
		if (cls == NULL) {ERROR("mid of <init> should not be null."); return NULL;}

		mid = (*env)->GetMethodID(env, cls, "<init>", "()V");
		if (mid == 0) {ERROR("mid of <init> should not be null."); return NULL;}
		board =	(*env)->NewObject(env, cls, mid);
		if (board == NULL) {ERROR("the generated board is null."); return NULL;}

		cells = (*env)->NewByteArray(env, BOARD_CELLS);
		(*env)->SetByteArrayRegion(env, cells, 0, BOARD_CELLS, (jbyte *)sb[i]->board);

		mid = (*env)->GetMethodID(env, cls, "setCells", "([B)V");
		if (mid == 0) {ERROR("mid of setCells should not be null."); return NULL;}

		(*env)->CallVoidMethod(env, board, mid, cells);

//		field_cells = (*env)->GetFieldID(env, cls, "cells", "[B");
//		(*env)->SetObjectField(env, board, field_cells, cells);

		(*env)->SetObjectArrayElement(env,ret,i,board);

		if ((*env)->ExceptionCheck(env) == JNI_TRUE) {
			ERROR("excption occurred.");
			(*env)->ExceptionClear(env);
			return NULL;
		}
		DEBUG("set start board array success at %d", i);
	}
	return ret;
}

JNIEXPORT jboolean JNICALL Java_zb_zxy_zyc_klotski_Game_start
  (JNIEnv *env, jobject obj)
{
	return (jboolean)start();
}

JNIEXPORT void JNICALL Java_zb_zxy_zyc_klotski_Game_end
  (JNIEnv *env, jobject obj)
{
	end();
}

JNIEXPORT jint JNICALL Java_zb_zxy_zyc_klotski_Game_doAnalysis
  (JNIEnv *env, jobject obj, jobject board)
{
	DEBUG("IN");
	jint ret = -1;
	jclass cls = NULL;
	jmethodID mid = 0;
	jbyteArray cells = NULL;
	jbyte *cs = NULL;

	if (!board) {ERROR("board is null");return ret;}

	cls = (*env)->GetObjectClass(env, board);
	if (cls == NULL) {ERROR("cls should not be null."); return ret;}

	mid = (*env)->GetMethodID(env, cls, "getCells", "()[B");
	if (mid == 0) {ERROR("mid should not be 0."); return ret;}

	cells = (jbyteArray)(*env)->CallObjectMethod(env, board, mid);
	if (cells == NULL) {ERROR("cells should not be null."); return ret;}

	cs = (*env)->GetByteArrayElements(env, cells, NULL);
	if (!cs) {
		ERROR("cs should not be null.");
		(*env)->ReleaseByteArrayElements(env, cells, cs, 0);
		return ret;
	}

	ret = (jint)doAnalysis((unsigned char*)cs);
	if (ret > 0) {
		mid = (*env)->GetMethodID(env, cls, "setCells", "([B)V");
		(*env)->SetByteArrayRegion(env, cells, 0, BOARD_CELLS, cs);
		(*env)->CallVoidMethod(env, board, mid, cells);
	}

	(*env)->ReleaseByteArrayElements(env, cells, cs, 0);

	if ((*env)->ExceptionCheck(env) == JNI_TRUE) {
		ERROR("excption occurred.");
		(*env)->ExceptionClear(env);
		return -1;
	}
	DEBUG("OUT");
	return ret;
}

JNIEXPORT jobject JNICALL Java_zb_zxy_zyc_klotski_Game_getNextBoard
  (JNIEnv *env, jobject obj, jobject board)
{
	jobject ret = NULL;
	jclass cls = NULL;
	jmethodID mid = 0;
	jbyteArray cells = NULL;
	jbyte *cs = NULL;
	jbyte *ncs = NULL;

	DEBUG("IN");

	cls = (*env)->GetObjectClass(env, board);
	if (cls == NULL) {ERROR("cls should not be null."); return ret;}

	mid = (*env)->GetMethodID(env, cls, "getCells", "()[B");
	if (mid == 0) {
		ERROR("mid should not be 0.");
		return NULL;
	}

	cells = (*env)->CallObjectMethod(env, board, mid);
	if (cells == NULL) {ERROR("cells should not be null."); return ret;}

	cs = (*env)->GetByteArrayElements(env, cells, NULL);
	ncs = (jbyte *)getNextBoard((unsigned char *)cs);
	(*env)->ReleaseByteArrayElements(env, cells, cs, 0);

	if (ncs) {
		cls = (*env)->FindClass(env, "zb/zxy/zyc/klotski/Board");
		if (cls == NULL) {ERROR("mid of <init> should not be null."); return NULL;}

		mid = (*env)->GetMethodID(env, cls, "<init>", "()V");
		if (mid == 0) {ERROR("mid of <init> should not be null."); return NULL;}

		ret =	(*env)->NewObject(env, cls, mid);
		if (ret == NULL) {ERROR("the generated board is null."); return NULL;}

		cells = (*env)->NewByteArray(env, BOARD_CELLS);
		(*env)->SetByteArrayRegion(env, cells, 0, BOARD_CELLS, ncs);

		mid = (*env)->GetMethodID(env, cls, "setCells", "([B)V");
		if (mid == 0) {ERROR("mid of setCells should not be null."); return NULL;}

		(*env)->CallVoidMethod(env, ret, mid, cells);
	} else {
		ERROR("next cells should not be null.");
	}

	if ((*env)->ExceptionCheck(env) == JNI_TRUE) {
		ERROR("excption occurred.");
		(*env)->ExceptionClear(env);
		return NULL;
	}

	DEBUG("OUT");
	return ret;
}

JNIEXPORT void JNICALL Java_zb_zxy_zyc_klotski_Game_printBoard
  (JNIEnv *env, jobject obj, jobject board)
{
	DEBUG("IN");

	jclass cls = NULL;
	jmethodID mid = 0;
	jbyteArray cells = NULL;
	jbyte *cs = NULL;

	cls = (*env)->GetObjectClass(env, board);
	mid = (*env)->GetMethodID(env, cls, "getCells", "()[B");
	if (mid == 0) {ERROR("mid should not be 0."); return;}

	cells = (jbyteArray)(*env)->CallObjectMethod(env, board, mid);
	if (cells == NULL) {ERROR("cells should not be null."); return;}

	cs = (*env)->GetByteArrayElements(env, cells, NULL);
	if (!cs) {
		ERROR("cs should not be null.");
		(*env)->ReleaseByteArrayElements(env, cells, cs, 0);
		return;
	}

	if ((*env)->ExceptionCheck(env) == JNI_TRUE) {
		ERROR("excption occurred.");
		(*env)->ExceptionClear(env);
		return;
	}

	printBoard(cs);
	(*env)->ReleaseByteArrayElements(env, cells, cs, 0);

	DEBUG("OUT");
}
