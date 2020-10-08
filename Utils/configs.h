#ifndef _Invoices_configs_h_
#define _Invoices_configs_h_
#include <CtrlLib/CtrlLib.h>
#include "conf.h"

using namespace Upp;
#include "../Tables/sqlincludes.h"

struct MyData {
	double	taxrate;
	String	dbfile;
	String	companyname, companyowner, companyaddress, companycity, 
		companystate, companyzip, companyphone, companyemail;
	
	void	Jsonize(JsonIO& json);
    void	Xmlize(XmlIO& xio)           { XmlizeByJsonize(xio, *this); }
};

struct Configs {
	
public:
	MyData	data;
	
	String	companyname, companyowner, companyaddress, companycity, 
		companystate, companyzip, companyphone, companyemail;
	
			Configs();
			~Configs();
	void 	Populate();
	void	SelectDB();
	void	SaveSettings();
	void	CreateSample();
	
	FileSel	selectdbwin;
	FileSel	selectodirwin;

};

#endif
