#ifndef _Invoices_Company_h_
#define _Invoices_Company_h_
#include "../Utils/configs.h"

#define LAYOUTFILE "Invoices/UI/Company.lay"
#include <CtrlCore/lay.h>

struct CompanyInfoWindow : WithCompanyInfoWindowLayout<TopWindow> {
	Configs myConfig;
	void Save() {	StoreAsJsonFile(myConfig.data); };
	void Cancel();
	public:
		typedef CompanyInfoWindow CLASSNAME;
		CompanyInfoWindow();
		void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};

#endif
