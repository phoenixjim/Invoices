#include "DBUI.h"
#include "../configs.h"

CompanyInfoWindow::CompanyInfoWindow()
{
	CtrlLayoutOKCancel(*this, "Create Invoice");
	ok << [=] { Save(); };
	cancel << [=] { Cancel(); };
	if(FileExists(myConfig.configfile))
	{
        VectorMap<String, String> cfg = LoadIniFile(myConfig.configfile);
        myConfig.DBFile = cfg.Get("DBFile", Null);
        myConfig.OutputDirectory = cfg.Get("OutputDirectory", Null);
        myConfig.companyowner = cfg.Get("companyowner", Null);
        myConfig.companyname = cfg.Get("companyname", Null);
        myConfig.companyaddress = cfg.Get("companyaddress", Null);
        myConfig.companycity = cfg.Get("companycity", Null);
        myConfig.companystate = cfg.Get("companystate", Null);
        myConfig.companyzip = cfg.Get("companyzip", Null);
        myConfig.companyphone = cfg.Get("companyphone", Null);
        myConfig.companyemail = cfg.Get("companyemail", Null);
	}
}

void CompanyInfoWindow::Save()
{
	if (txtBusName.GetData().IsNull() || txtBusOwner.GetData().IsNull()
	|| txtBusAddress.GetData().IsNull() || txtBusCity.GetData().IsNull() 
	|| txtBusState.GetData().IsNull() || txtBusZip.GetData().IsNull() 
	|| txtBusPhone.GetData().IsNull() || txtBusEmail.GetData().IsNull())
		return;
	
	myConfig.cfg << "companyname=" << txtBusName.GetData().ToString();
	myConfig.cfg << "companyowner=" << txtBusOwner.GetData().ToString();
	myConfig.cfg << "companyaddress=" << txtBusAddress.GetData().ToString();
	myConfig.cfg << "companycity=" << txtBusCity.GetData().ToString();
	myConfig.cfg << "companystate=" << txtBusState.GetData().ToString();
	myConfig.cfg << "companyzip=" << txtBusZip.GetData().ToString();
	myConfig.cfg << "companyphone=" << txtBusPhone.GetData().ToString();
	myConfig.cfg << "companyemail=" << txtBusEmail.GetData().ToString();

	if(!SaveFile(ConfigFile(), myConfig.cfg))
	{
    	Exclamation("Error saving configuration!");
	}
	Close();
}

void CompanyInfoWindow::Cancel()
{
	Close();
}
