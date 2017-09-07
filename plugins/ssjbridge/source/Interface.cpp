#ifndef DLLEXP
#if _WIN32|_WIN64
#define DLLEXP extern "C" __declspec( dllexport )
#else
#define DLLEXP extern "C" __attribute__((visibility("default")))
#endif
#endif

#include <jni.h>
#include "ssi.h"

DLLEXP long JNICALL Java_hcm_ssj_mobileSSI_SSI_clear(JNIEnv *env, jobject thisObj)
{
    ssi::Factory::Clear();
}

ssi_type_t getStreamType(JNIEnv *env, jobject type)
{
    jmethodID method_stream_type_ordinal = env->GetMethodID(env->GetObjectClass(type), "ordinal", "()I");
    jint type_ordinal = env->CallIntMethod(type, method_stream_type_ordinal);

    switch(type_ordinal) {
        default:
        case 0: //UNDEF
            return SSI_UNDEF;
        case 1: //BYTE
            return SSI_CHAR;
        case 2: //CHAR
            return SSI_CHAR;
        case 3: //SHORT
            return SSI_SHORT;
        case 4: //INT
            return SSI_INT;
        case 5: //LONG
            return SSI_LONG;
        case 6: //FLOAT
            return SSI_FLOAT;
        case 7: //DOUBLE
            return SSI_DOUBLE;
        case 8: //BOOL
            return SSI_BOOL;
        case 9: //STRING
            return SSI_UNDEF;
        case 10: //IMAGE
            return SSI_IMAGE;
    }
}

void createStream(JNIEnv *env, jobject ssj_stream, ssi_stream_t& ssi_stream)
{
    jclass class_stream = env->GetObjectClass(ssj_stream);

    jint dim = env->GetIntField(ssj_stream, env->GetFieldID(class_stream, "dim", "I"));
    jint num = env->GetIntField(ssj_stream, env->GetFieldID(class_stream, "num", "I"));
    jint bytes = env->GetIntField(ssj_stream, env->GetFieldID(class_stream, "bytes", "I"));

    jdouble sr = env->GetDoubleField(ssj_stream, env->GetFieldID(class_stream, "sr", "D"));
    jdouble time = env->GetDoubleField(ssj_stream, env->GetFieldID(class_stream, "time", "D"));

    ssi_stream_init(ssi_stream, 0, (ssi_size_t)dim, (ssi_size_t)bytes, SSI_UNDEF, sr, time);
    ssi_stream.num = (ssi_size_t)num;

    jfieldID field_stream_type = env->GetFieldID(class_stream, "type", "Lhcm/ssj/core/Cons$Type;");
    jobject ssj_type = env->GetObjectField(ssj_stream, field_stream_type);
    ssi_stream.type = getStreamType(env, ssj_type);
}

DLLEXP long JNICALL Java_hcm_ssj_mobileSSI_SSI_create(JNIEnv *env, jobject thisObj, jstring name, jstring libname, jstring libpath) {

	const char* name_cstr = env->GetStringUTFChars(name, NULL);
    const char* libname_cstr = env->GetStringUTFChars(libname, NULL);
    const char* libpath_cstr = env->GetStringUTFChars(libpath, NULL);

    ssi::Factory::GetApp()->setLibDir((char*)libpath_cstr);
	ssi::Factory::RegisterDLL(libname_cstr);
	ssi::IObject* obj = ssi::Factory::Create(name_cstr, 0, true);

    env->ReleaseStringUTFChars(name, name_cstr);
    env->ReleaseStringUTFChars(libname, libname_cstr);
    env->ReleaseStringUTFChars(libpath, libpath_cstr);

	return (long) obj;
}

DLLEXP jboolean JNICALL Java_hcm_ssj_mobileSSI_SSI_setOption(JNIEnv *env, jobject thisObj, jlong ssiobj, jstring name, jstring value) {

    const char* name_cstr = env->GetStringUTFChars(name, NULL);
    const char* value_cstr = env->GetStringUTFChars(value, NULL);

    bool res = ((ssi::IObject*) ssiobj)->getOptions()->setOptionValueFromString(name_cstr, value_cstr);

    env->ReleaseStringUTFChars(name, name_cstr);
    env->ReleaseStringUTFChars(value, value_cstr);

    return (jboolean)res;
}

DLLEXP void JNICALL Java_hcm_ssj_mobileSSI_SSI_transformEnter(JNIEnv *env, jobject thisObj, jlong ssiobj, jobject ssj_stream_in, jobject ssj_stream_out) {

	using namespace ssi;
	ITransformer* transformer = (ITransformer*) ssiobj;

    jclass class_stream = env->GetObjectClass(ssj_stream_in);

	/*
     * CREATE STREAM IN
     */
	ssi_stream_t ssi_stream_in;
    createStream(env, ssj_stream_in, ssi_stream_in);

	/*
     * CREATE STREAM OUT
     */
	ssi_stream_t ssi_stream_out;
	ssi_stream_init(ssi_stream_out, 0,
					transformer->getSampleDimensionOut(ssi_stream_in.dim),
					transformer->getSampleBytesOut(ssi_stream_in.byte),
					transformer->getSampleTypeOut(ssi_stream_in.type),
                    env->GetDoubleField(ssj_stream_out, env->GetFieldID(class_stream, "sr", "D")));
    ssi_stream_out.num = transformer->getSampleNumberOut(ssi_stream_in.num);

	/*
     * TRANSFORM THE DATA
     */
	transformer->transform_enter(ssi_stream_in, ssi_stream_out);
}

DLLEXP void JNICALL Java_hcm_ssj_mobileSSI_SSI_transform(JNIEnv *env, jobject thisObj, jlong ssiobj, jobject ssj_stream_in, jobject ssj_stream_out) {

    using namespace ssi;
    ITransformer* transformer = (ITransformer*) ssiobj;

    /*
     * CREATE STREAM IN
     */
    ssi_stream_t ssi_stream_in;
    createStream(env, ssj_stream_in, ssi_stream_in);

    jclass class_stream = env->GetObjectClass(ssj_stream_in);
    switch(ssi_stream_in.type)
    {
        default:
        case SSI_UNDEF:
            break;
        case SSI_CHAR:
        {
            jmethodID method_stream_ptr = env->GetMethodID(class_stream, "ptrB", "()[B");
            jobject data = env->CallObjectMethod(ssj_stream_in, method_stream_ptr);
            jbyteArray arr = reinterpret_cast<jbyteArray>(&data);
            ssi_stream_in.ptr = (ssi_byte_t *) env->GetByteArrayElements(arr, NULL);
            ssi_stream_in.type = SSI_CHAR;
            break;
        }
        case SSI_FLOAT: {
            jmethodID method_stream_ptr = env->GetMethodID(class_stream, "ptrF", "()[F");
            jobject data = env->CallObjectMethod(ssj_stream_in, method_stream_ptr);
            jfloatArray arr = reinterpret_cast<jfloatArray>(data);
            ssi_stream_in.ptr = (ssi_byte_t *) env->GetFloatArrayElements(arr, NULL);
            ssi_stream_in.type = SSI_FLOAT;
            break;
        }
    }

    /*
     * CREATE STREAM OUT
     */
    ssi_stream_t ssi_stream_out;
    jdouble sr_out = env->GetDoubleField(ssj_stream_out, env->GetFieldID(class_stream, "sr", "D"));

    ssi_stream_init(ssi_stream_out,
                    transformer->getSampleNumberOut(ssi_stream_in.num),
                    transformer->getSampleDimensionOut(ssi_stream_in.dim),
                    transformer->getSampleBytesOut(ssi_stream_in.byte),
                    transformer->getSampleTypeOut(ssi_stream_in.type),
                    sr_out);

    /*
     * TRANSFORM THE DATA
     */
    ITransformer::info tinfo; //todo
    tinfo.delta_num = 0;
    tinfo.frame_num = 0;
    tinfo.time = ssi_stream_in.time;

    transformer->transform(tinfo, ssi_stream_in, ssi_stream_out);

    /*
     * PASS DATA BACK TO SSJ
     */
    switch(ssi_stream_in.type)
    {
        default:
        case SSI_UNDEF:
            break;
        case SSI_CHAR:
        {
            jmethodID method_stream_ptr = env->GetMethodID(class_stream, "ptrB", "()[B");
            jobject data = env->CallObjectMethod(ssj_stream_out, method_stream_ptr);
            jbyteArray arr = reinterpret_cast<jbyteArray>(&data);
            env->SetByteArrayRegion(arr, 0, ssi_stream_out.num * ssi_stream_out.dim, (jbyte*)ssi_stream_out.ptr);
            break;
        }
        case SSI_FLOAT: {
            jmethodID method_stream_ptr = env->GetMethodID(class_stream, "ptrF", "()[F");
            jobject data = env->CallObjectMethod(ssj_stream_out, method_stream_ptr);
            jfloatArray arr = reinterpret_cast<jfloatArray>(data);
            env->SetFloatArrayRegion(arr, 0, ssi_stream_out.num * ssi_stream_out.dim, (float*)ssi_stream_out.ptr);

            break;
        }
    }
}

DLLEXP void JNICALL Java_hcm_ssj_mobileSSI_SSI_transformFlush(JNIEnv *env, jobject thisObj, jlong ssiobj, jobject ssj_stream_in, jobject ssj_stream_out) {

    using namespace ssi;
    ITransformer* transformer = (ITransformer*) ssiobj;

    jclass class_stream = env->GetObjectClass(ssj_stream_in);

    /*
     * CREATE STREAM IN
     */
    ssi_stream_t ssi_stream_in;
    createStream(env, ssj_stream_in, ssi_stream_in);

    /*
     * CREATE STREAM OUT
     */
    ssi_stream_t ssi_stream_out;
    ssi_stream_init(ssi_stream_out, 0,
                    transformer->getSampleDimensionOut(ssi_stream_in.dim),
                    transformer->getSampleBytesOut(ssi_stream_in.byte),
                    transformer->getSampleTypeOut(ssi_stream_in.type),
                    env->GetDoubleField(ssj_stream_out, env->GetFieldID(class_stream, "sr", "D")));
    ssi_stream_out.num = transformer->getSampleNumberOut(ssi_stream_in.num);

    /*
     * TRANSFORM THE DATA
     */
    transformer->transform_flush(ssi_stream_in, ssi_stream_out);
}

DLLEXP jint JNICALL Java_hcm_ssj_mobileSSI_SSI_getSampleNumberOut(JNIEnv *env, jobject thisObj, jlong ssiobj, jint sample_number_in)
{
    using namespace ssi;
    ITransformer* transformer = (ITransformer*) ssiobj;
    return transformer->getSampleNumberOut((ssi_size_t)sample_number_in);
}

DLLEXP jint JNICALL Java_hcm_ssj_mobileSSI_SSI_getSampleDimensionOut(JNIEnv *env, jobject thisObj, jlong ssiobj, jint sample_dimension_in)
{
    using namespace ssi;
    ITransformer* transformer = (ITransformer*) ssiobj;
    return transformer->getSampleDimensionOut((ssi_size_t)sample_dimension_in);
}

DLLEXP jint JNICALL Java_hcm_ssj_mobileSSI_SSI_getSampleTypeOut(JNIEnv *env, jobject thisObj, jlong ssiobj, jobject sample_type_in)
{
    using namespace ssi;
    ITransformer* transformer = (ITransformer*) ssiobj;
    return transformer->getSampleTypeOut(getStreamType(env, sample_type_in));
}

DLLEXP jint JNICALL Java_hcm_ssj_mobileSSI_SSI_getSampleBytesOut(JNIEnv *env, jobject thisObj, jlong ssiobj, jint sample_bytes_in)
{
    using namespace ssi;
    ITransformer* transformer = (ITransformer*) ssiobj;
    return transformer->getSampleBytesOut((ssi_size_t)sample_bytes_in);
}

