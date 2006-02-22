#include <TAP.h>
#include <artoolkit.h>


arToolkit *state;

int main(int argc,char** argv) {

	tpAutoPtr<tpVideoSourcePlugin> plugin;
	tpVideoFrame frame;

	tpThePluginManager->addPath("."); 

	plugin = reinterpret_cast<tpVideoSourcePlugin*>(tpThePluginManager->getPlugin("video:capture:directshow")); 

	if (!plugin.isValid()) {
		tpLogError("Could not load!");
		exit(0);
	}; 

	tpVideoSourceInfo vsource;
	
	vsource.id = 0;
	vsource.show_source = TRUE;
	vsource.show_filter = TRUE;
	vsource.fps = 15;
	
	vsource.format = TP_YUV420;
	vsource.width = 320;
	vsource.height = 240;		
	vsource.devname = "/dev/video0"; 


	if (plugin->create(vsource,frame))
	{
		tpLogMessage("Capturing from %s with %dx%d",
			vsource.fullname,frame.width,frame.height);
	} else tpLogError("Could Not Create");

	if (!plugin->start()) tpLogError("Could Not Start!"); 


};