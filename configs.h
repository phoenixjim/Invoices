#ifndef _Invoices_configs_h_
#define _Invoices_configs_h_
#include <CtrlLib/CtrlLib.h>

using namespace Upp;


struct Configs {
public:
	String	GetOutputDirectory();
	String	SelectDB();
	void 	Initialize();
	
	String	DBFile;
	String	OutputDirectory;
	String	configfile = ConfigFile();
	String	cfg;

	String	companyname, companyowner, companyaddress, companycity, companystate, companyzip,
	companyphone, companyemail;
	
	// long lastinvoice; // Discover when needed
	
	FileSel	selectdbwin;
	FileSel	selectodirwin;

};

#endif
