#include "DBUI.h"
#include "../Utils/configs.h"

CompanyInfoWindow::CompanyInfoWindow()
{
	CtrlLayoutOKCancel(*this, "Set Company Info");
	ok << [=] { Save(); Close();};
	cancel << [=] { Cancel(); };

	LoadFromJsonFile(myConfig.data);
	txtBusName.SetData(myConfig.data.companyname);
	txtBusOwner.SetData(myConfig.data.companyowner);
	txtBusAddress.SetData(myConfig.data.companyaddress);
	txtBusCity.SetData(myConfig.data.companycity);
	txtBusState.SetData(myConfig.data.companystate);
	txtBusZip.SetData(myConfig.data.companyzip);
	txtBusPhone.SetData(myConfig.data.companyphone);
	txtBusEmail.SetData(myConfig.data.companyemail);
	txtTaxrate.SetData(round((double)myConfig.data.taxrate)/10000);
}

void CompanyInfoWindow::Cancel()
{
	Close();
}

void CompanyInfoWindow::Save()
{
	myConfig.data.taxrate = round((double)txtTaxrate.GetData()*10000.00);
	myConfig.data.companyname = ~txtBusName.GetData();
	myConfig.data.companyowner = ~txtBusOwner.GetData();
	myConfig.data.companyaddress = ~txtBusAddress.GetData();
	myConfig.data.companycity = ~txtBusCity.GetData();
	myConfig.data.companystate = ~txtBusState.GetData();
	myConfig.data.companyzip = ~txtBusZip.GetData();
	myConfig.data.companyphone = ~txtBusPhone.GetData();
	myConfig.data.companyemail = ~txtBusEmail.GetData();
	
	myConfig.SaveSettings();
}