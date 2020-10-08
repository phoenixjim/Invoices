#include "Company.h"
#include "../Utils/configs.h"

CompanyInfoWindow::CompanyInfoWindow()
{
	CtrlLayoutOKCancel(*this, "Create Invoice");
	ok << [=] { Save(); };
	cancel << [=] { Cancel(); };
	
	if ( !FileExists ( ConfigFile(GetExeTitle() + ".json") ) )
	{
		myConfig.data.taxrate = 0.08;
		myConfig.data.companyname = "Company Name";
		myConfig.data.companyowner = "James Lefavour";
		myConfig.data.companyaddress = "0 Main Way";
		myConfig.data.companycity = "City";
		myConfig.data.companystate = "State";
		myConfig.data.companyzip = "00000";
		myConfig.data.companyphone = "(555) 555 - 1212";
		myConfig.data.companyemail = "jim@myemail.com";
		Save();
	}

}

void CompanyInfoWindow::Cancel()
{
	Close();
}
