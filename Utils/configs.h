#ifndef _Invoices_configs_h_
#define _Invoices_configs_h_
#include <CtrlLib/CtrlLib.h>

using namespace Upp;
#include "../Tables/sqlincludes.h"

struct MyData {
	double	taxrate;
	String	dbfile;
	String	companyname, companyowner, companyaddress, companycity, 
		companystate, companyzip, companyphone, companyemail;
	
	void	Jsonize(JsonIO& json);
    // void	Xmlize(XmlIO& xio)           { XmlizeByJsonize(xio, *this); }
};

struct Configs {
	
public:
	MyData	data;
			Configs();
//			~Configs();
	void 	Populate();
	void	SelectDB();
	void	SaveSettings();

	FileSel	selectdbwin;

};

#endif
