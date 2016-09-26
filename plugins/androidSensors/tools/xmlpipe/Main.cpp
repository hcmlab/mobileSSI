// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>, Simon Flutura <Simon.Flutura@informatik.uni-augsburg.de>
// created: 2011/03/29
// Copyright (C) University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#include "ssi.h"
/*
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>*/
#include "androidSensors/include/ssiandroidsensors.h"
#include "androidJavaSensors/include/ssiandroidjavasensors.h"
#include <unistd.h>
#include "zip.h"
#include <thread>
#include <stdio.h>
#include <time.h>
#include <dirent.h>

using namespace ssi;

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif
#if __gnu_linux__
#include <unistd.h>
#endif





//hackish global variable for android main loop
bool loop=true;

#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <android/log.h>

#define APPNAME "android_xmlpipe"



/**
 * Extract file from apk using android asset manager 
 **/


int getMeMaPipeAsset(AAssetManager* mgr, char* appPath, char* assetName)
{
	__android_log_print(ANDROID_LOG_INFO, APPNAME,"###ze\n");
	//AAssetManager mgr = AAssetManager_fromJava(JNIEnv* env, jobject assetManager);
	
	

	//open asset
	AAsset* myPipe = AAssetManager_open(mgr, assetName, 0);
	int nb_read = 0;
	char* filePath= new char[strlen(appPath)+ strlen(assetName)+1];
	strcpy(filePath, appPath);
	filePath=strcat(filePath, assetName);
	FILE* out = fopen(filePath, "w");
	
	__android_log_print(ANDROID_LOG_INFO, APPNAME,"###un%s\n", filePath);
	int i=0;
	//read asset and write to file

	fwrite(AAsset_getBuffer(myPipe), 1, AAsset_getLength(myPipe), out);
	
	__android_log_print(ANDROID_LOG_INFO, APPNAME,"###d1o\n");
	fclose(out);
	__android_log_print(ANDROID_LOG_INFO, APPNAME,"###d2o\n");
	 AAsset_close(myPipe);
 
 

	__android_log_print(ANDROID_LOG_INFO, APPNAME,"###do\n");

	
	
	
	
	//fseek  SEEK_END to get correct size
	FILE* in=fopen(filePath, "r");
	fseek(in, 0L, SEEK_END);
	i=ftell(in);
	rewind(in);
	__android_log_print(ANDROID_LOG_INFO, APPNAME,"fileLEN?: %d\n", i);
	
	//read file for test
	char* buff=new char[i];
	fread(buff, 1, i, in);
	
	 __android_log_print(ANDROID_LOG_INFO, APPNAME,"file?: %s\n", buff);
	  
	 
	__android_log_print(ANDROID_LOG_INFO, APPNAME,"fileEND?: \n");
	delete[] buff;
	delete[] filePath;
	
	
	
}


// assenmgr, where we are now(changes as we descend), new directory (will be created)
int extractFilesFromDir(AAssetManager* mgr, char* appPath, char* dirName)
{
	AAssetDir* assetDir= AAssetManager_openDir( mgr, dirName);
	//is no directory
	if(!assetDir)return 0;
	

	const char* filename=0;
	char ucFilename[SSI_MAX_CHAR];
	char appath2[SSI_MAX_CHAR];
	char* appath=appath2;
	char* lastSubDir=0;
	int i;
	for(i= strlen(dirName);(dirName[i]=='/')||(!i) ;i--);
	
	lastSubDir=dirName+i;
	strcpy(appath, appPath);
	appath=strcat( appath, lastSubDir);
	
	mkdir(appath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	
	__android_log_print(ANDROID_LOG_INFO, APPNAME, "# %s ",appath );
	
	for(filename=AAssetDir_getNextFileName(assetDir);
		filename;
		filename=AAssetDir_getNextFileName(assetDir)
		)
	{
		strcpy(ucFilename, filename);
		
		if(! extractFilesFromDir(mgr, appPath, ucFilename))
		{
			//we have an asset?
			getMeMaPipeAsset(mgr,  appPath, ucFilename);
	 	}

	 	 extractFilesFromDir(mgr, appPath, ucFilename);
	}
	
	AAssetDir_close(assetDir);
	
	return 1;
}


/**
    move sessions files
 */

bool DirectoryExists( const char* pzPath )
{
    if ( pzPath == NULL) return false;

    DIR *pDir;
    bool bExists = false;

    pDir = opendir (pzPath);

    if (pDir != NULL)
    {
        bExists = true;
        (void) closedir (pDir);
    }

    return bExists;
}



template < class CLOCK = std::chrono::steady_clock >
auto to_string_iso8601 ( const typename CLOCK::time_point& t = CLOCK::now() )
  -> std::string
{
      using namespace std::chrono;
      const time_t ct
      ( ( duration_cast< seconds > ( system_clock::now( ).time_since_epoch( ) )
      + duration_cast< seconds > ( t - CLOCK::now( ) )
        ).count( )
      );
      std::string r = "0000-00-00T00-00-00Z.";
    #ifdef _WIN32
      // TDM-GCC-5.1.0 is not support %F and %T
      //   note: mingw is supported. but we cannot predicate TDM or not.
      constexpr auto format = "%Y-%m-%dT%H-%M-%SZ";
    #else
      //constexpr auto format = "%FT%TZ";
       constexpr auto format = "%Y-%m-%dT%H-%M-%SZ";
    #endif
      std::strftime ( const_cast< char* > ( r.data( ) ), r.size( ), format, std::gmtime ( &ct ) );
      return r;
}




//rename record folder according to timestamp
void postProcessFiles()
{

      std::string timestamp=to_string_iso8601();

     if(DirectoryExists("record")){
     rename("record", timestamp.c_str());
     mkdir("/sdcard/android_xmlpipe/record", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
     }

}



/**
 * XMLpipe's core function see core/tools
 **/

void RunMachine()
{

}


void RunSSI ( ssi_char_t *exepath,  ssi_char_t *filepath,
            ssi_char_t *configpaths, ssi_char_t *libsPath,
            bool savepipe,  bool init_only, bool export_dlls, bool show_close_but
          )
{	


	ssi_char_t _workdir[SSI_MAX_CHAR];
	__android_log_print(ANDROID_LOG_INFO, APPNAME, "#########XML Pipe thread is here %s ########",libsPath);
	
	if(!Factory::GetApp())
	{
		__android_log_print(ANDROID_LOG_INFO, APPNAME, "Factory::GetFactory()->getApp() is null");
		Factory::Clear ();
		return;
	}
	
	Factory::GetApp()->setLibDir(libsPath);
	ssimsg=new ssi::FileMessage("/sdcard/android_xmlpipe/ssiLog");

        bool import=true;

        import=import&&Factory::RegisterDLL ("ssiframe", ssiout, ssimsg);
        import=import&&Factory::RegisterDLL ("ssievent", ssiout, ssimsg);
        import=import&&Factory::RegisterDLL ("ssiandroidsensors", ssiout, ssimsg);
        
        if(!import)
        {
			__android_log_print(ANDROID_LOG_INFO, APPNAME, "importing shared libraries faild");
			Factory::Clear ();
		return;
		}
		
		
		
__android_log_print(ANDROID_LOG_INFO, APPNAME, "#########1########");
        //needed for AndroidMessage

        // forward ssi messages to android logcat
        //ssimsg=new ssi::AndroidMessage();

__android_log_print(ANDROID_LOG_INFO, APPNAME, "#########2########");
#if _WIN32||_WIN64
	// disable close button
	if (!show_close_but) {
		RemoveCloseButton ();
	}

	// working directory
	::GetCurrentDirectory (SSI_MAX_CHAR, _workdir);
#else

	getcwd(_workdir,SSI_MAX_CHAR);
	__android_log_print(ANDROID_LOG_INFO, APPNAME, "#########3########");
#endif
	// executable directory
	FilePath exepath_fp (exepath);	
	ssi_char_t exedir[SSI_MAX_CHAR];
	if (exepath_fp.isRelative ()) {

		ssi_sprint (exedir, "%s/%s", _workdir, exepath_fp.getDir ());

	} else {
		strcpy (exedir, exepath_fp.getDir ());
	}
__android_log_print(ANDROID_LOG_INFO, APPNAME, "#########4########");
	// full pipepath
	FilePath filepath_fp (filepath);	
	ssi_char_t pipepath[SSI_MAX_CHAR];
	memset(pipepath,0, SSI_MAX_CHAR );
	if (filepath_fp.isRelative ()) {

		ssi_sprint (pipepath, "%s/%s", _workdir, filepath);
		
__android_log_print(ANDROID_LOG_INFO, APPNAME, "#%s, %s",pipepath, filepath);


	} else {
		strcpy (pipepath, filepath);
	}

	// set working directory to pipeline directory

	chdir(filepath_fp.getDir ());


__android_log_print(ANDROID_LOG_INFO, APPNAME, "#########5#######");
	XMLPipeline *xmlpipe = ssi_create (XMLPipeline, 0, true);
__android_log_print(ANDROID_LOG_INFO, APPNAME, "#########5.1#######");

	if(xmlpipe==0)
	{
		__android_log_print(ANDROID_LOG_INFO, APPNAME, "#this would go wrong, xmlpipe is NULL");
		
		Factory::Clear ();
		return;
		
	}
	xmlpipe->SetRegisterDllFptr (Factory::RegisterDLL);
        __android_log_print(ANDROID_LOG_INFO, APPNAME, "#########5.5#######");
        __android_log_print(ANDROID_LOG_INFO, APPNAME, pipepath);
        __android_log_print(ANDROID_LOG_INFO, APPNAME, filepath);
        __android_log_print(ANDROID_LOG_INFO, APPNAME, "#########5.6#######");
	bool result = false;
	if (configpaths) {
		ssi_size_t n = 0;
		n = ssi_split_string_count (configpaths, ';');
		ssi_char_t **ns = new ssi_char_t *[n];
		ssi_split_string (n, ns, configpaths, ';');
		result = xmlpipe->parse (pipepath, n, ns, savepipe);
	} else {
		result = xmlpipe->parse (pipepath, 0, 0, savepipe);
	}
__android_log_print(ANDROID_LOG_INFO, APPNAME, "#########6#######");
	if (!result) {
		ssi_print ("ERROR: could not parse pipeline from '%s'\n", pipepath);
		ssi_print ("\n\n\t\tpress enter to quit\n");
		getchar ();
	} else {

		if (export_dlls) {
			Factory::ExportDlls (exedir);
		}

		if (!init_only) {			

			ITheFramework *frame = Factory::GetFramework ();
			frame->SetLogLevel(SSI_LOG_LEVEL_VERBOSE);

			ITheEventBoard *eboard = 0;
			if (xmlpipe->startEventBoard ()) {
				eboard = Factory::GetEventBoard ();
			}						
			frame->Start ();
			if (eboard) {
				eboard->Start ();
			}
			
__android_log_print(ANDROID_LOG_INFO, APPNAME, "#########runnin#######");
			frame->Wait ();
			if (eboard) {
				eboard->Stop ();
			}
			frame->Stop ();			
			frame->Clear ();
			if (eboard) {
				eboard->Clear ();
			}

			
		} else {
			ssi_print ("SUCCESS: initialization ok");
			ssi_print ("\n\n\t\tpress enter to quit\n");
			getchar ();
			
		}
	}
__android_log_print(ANDROID_LOG_INFO, APPNAME, "#########9#######");
	Factory::Clear ();
	#if _WIN32||_WIN64
	::SetCurrentDirectory (_workdir);
	#else
	chdir(_workdir);
        xmlpipe=NULL;
	#endif

        postProcessFiles();
	delete[] exepath;
    delete[] filepath;
    delete[] libsPath;
}









extern "C" {
	
        jint JNI_OnLoad(JavaVM* aVm, void* aReserved)
        {
                return JNI_VERSION_1_6;
        }
        /* wrapers for empathica test init */

        void Java_hcm_ssj_core_Util_stopSSI(JNIEnv * env,jobject thiz)
        {
            if(Factory::GetFramework ())
                            Factory::GetFramework ()->CancelWait();
            // stop ssi stray ssi instance that should not be
            else Factory::Clear();
        }

        void Java_de_hcmlab_ssi_android_1xmlpipe_SensorCollectorService_startSSI
                        (JNIEnv * env,jobject thiz, jstring jpath, jobject assetManager, jboolean extractFiles);

        void Java_hcm_ssj_core_Util_startSSI(JNIEnv * env,jobject thiz, jstring jpath, jobject assetManager, jboolean extractFiles)
        {
            Java_de_hcmlab_ssi_android_1xmlpipe_SensorCollectorService_startSSI(env, thiz, jpath, assetManager, extractFiles);
        }





        void Java_de_hcmlab_ssi_android_1xmlpipe_SensorCollectorService_stopSSI
                                                        (JNIEnv * env,jobject thiz)
        {
                if(Factory::GetFramework ())
                                Factory::GetFramework ()->CancelWait();
                // stop ssi stray ssi instance that should not be
                else Factory::Clear();


        }
        void Java_de_hcmlab_ssi_android_1xmlpipe_SensorCollectorService_startSSI
                        (JNIEnv * env,jobject thiz, jstring jpath, jobject assetManager, jboolean extractFiles)
        {

                const char* app_dir = env->GetStringUTFChars(jpath, NULL);

                char* libsPath= new char[strlen(app_dir)+strlen("/lib/")+1];
                char* execPath= new char[strlen(app_dir)+strlen("/cache/")+1];
                char* filePath= new char[strlen(app_dir)+strlen("androidSensors.pipeline")+1];
                char* filePathHTML= new char[strlen(app_dir)+strlen("index.html")+1];
                char* dirName=new char[+strlen("/stimuli")+1];

                memset(libsPath, 0, strlen(app_dir)+strlen("/lib/")+1);
                memset(execPath, 0, strlen(app_dir)+strlen("/cache/")+1);
                memset(filePath, 0, strlen(app_dir)+strlen("androidSensors.pipeline")+1);
                memset(filePathHTML, 0, strlen(app_dir)+strlen("index.html")+1);
                memset(dirName, 0, strlen("/stimuli")+1);

                strncpy(libsPath, app_dir, strlen(app_dir) - strlen("/cache"));

                strcpy(execPath, "/sdcard/android_xmlpipe");
                strcpy(filePath, "androidSensors.pipeline");
                strcpy(filePathHTML, "index.html");
                strcpy(dirName, "/stimuli");


                // set all necessary paths
                libsPath=strcat(libsPath, "/lib/");
                execPath=strcat(execPath, "/");


                env->ReleaseStringUTFChars( jpath, app_dir);

                // chdir in the application cache directory
                __android_log_print(ANDROID_LOG_INFO, APPNAME,"libpathr: %s", libsPath);


                //create directory on sdcard rw for user and group, r for others
                mkdir("/sdcard/android_xmlpipe", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                chdir("/sdcard/android_xmlpipe");


                if(extractFiles==JNI_TRUE)
                {
                // free string allocated by java

        __android_log_print(ANDROID_LOG_INFO, APPNAME,"now extract assets");

                //jobject assetManager = state->activity->assetManager;
                AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);




                    __android_log_print(ANDROID_LOG_INFO, APPNAME, "pathses %s %s %s %d", execPath, filePath, libsPath, extractFiles);
                    getMeMaPipeAsset(mgr, execPath, filePath);


                }

                __android_log_print(ANDROID_LOG_INFO, APPNAME, "im alive %s %s %s", execPath, filePath, execPath);
                //run xmlpipe main


                std::thread t(RunSSI, execPath, filePath, execPath, libsPath, false, false, false, false );

                //std::thread t(main_ml, libsPath);

                t.detach();


                        }
			
}
