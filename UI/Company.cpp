#include "DBUI.h"
#include "../configs.h"

CompanyInfoWindow::CompanyInfoWindow()
{
	CtrlLayoutOKCancel(*this, "Create Invoice");
	ok << [=] { Save(); };
	cancel << [=] { Cancel(); };
}

void CompanyInfoWindow::Save()
{
	Close();
}

void CompanyInfoWindow::Cancel()
{
	Close();
}
