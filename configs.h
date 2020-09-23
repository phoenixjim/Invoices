#ifndef _Invoices_configs_h_
#define _Invoices_configs_h_
#include <CtrlLib/CtrlLib.h>

using namespace Upp;


struct Configs {
	
	String	GetOutputDirectory();
	String	SelectDB();

	String	DBFile;
	String	OutputDirectory;
	String	configfile = ConfigFile();
	String	cfg;

	FileSel	selectdbwin;
	FileSel	selectodirwin;

};

#endif
