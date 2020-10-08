#ifndef _Invoices_configs_h_
#define _Invoices_configs_h_
#include <CtrlLib/CtrlLib.h>
#include "conf.h"

using namespace Upp;
#include "../Tables/sqlincludes.h"

struct Configs {
public:
	String	DBFile;
	String	OutputDirectory;
	String	configfile;
	config	cfg;

	String	companyname, companyowner, companyaddress, companycity, 
		companystate, companyzip, companyphone, companyemail;
	
			Configs();
			~Configs();
	void	GetOutputDirectory();
	void 	Populate();
	void	SelectDB();
	void	SaveSettings();
	void	CreateSample();
	
	FileSel	selectdbwin;
	FileSel	selectodirwin;

};

#endif
