#include "DBUI.h"
#include "../Utils/configs.h"

CompanyInfoWindow::CompanyInfoWindow()
{
	CtrlLayoutOKCancel(*this, "Create Invoice");
	ok << [=] { Save(); };
	cancel << [=] { Cancel(); };
}

void CompanyInfoWindow::Save()
{
	Exclamation("Not yet implemented, please hand edit config.");
	Close();
}

void CompanyInfoWindow::Cancel()
{
	Close();
}
