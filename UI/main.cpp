#include "Invoices.h"

#define MODEL "Invoices/Tables/Invoices.sch"
#include <Sql/sch_schema.h>
#undef MODEL

void Invoices::MainMenu(Bar& bar)
{
	bar.Add(t_("Customers"),  [=]{ if(!cwin.IsOpen()) cwin.Open(this); });

    bar.Add(t_("Products"), [=]{ if(!prodwin.IsOpen()) prodwin.Open(this); });
	
	bar.Sub(t_("Transactions"), THISFN(TransactionsMenu));
//	bar.Sub(t_("Reports"), THISFN(ReportsMenu));
	bar.Sub(t_("Management"), THISFN(ManagementMenu));
	bar.Add(t_("Exit"), THISFN(Close));
}

void Invoices::TransactionsMenu(Bar& bar)
{
	bar.Add(t_("Create Invoice"), [=]{ if(!createinvoicewin.IsOpen()) 	createinvoicewin.Open(this); });
	bar.Add(t_("List Invoices"), [=]{ if(!invoiceswin.IsOpen()) 	invoiceswin.Open(this); });
	bar.Add(t_("List Line Items"), [=]{ if(!listlineitemswin.IsOpen()) 	listlineitemswin.Open(this); });
}
/*
void Invoices::ReportsMenu(Bar& bar)
{
	bar.Add(t_("Income / Sales Tax"), [=]{ if(!taxreportwin.IsOpen()) taxreportwin.Open(this); });
	bar.Add(t_("Income / Sales Tax by Customer"), [=]{ if(!taxbycustomerwin.IsOpen()) taxbycustomerwin.Open(this); });
	bar.Add(t_("Income by Customer"), [=]{ if(!incomewin.IsOpen()) incomewin.Open(this); });
	bar.Add(t_("Profit / Loss"), [=]{ if(!profitwin.IsOpen()) profitwin.Open(this); });
}
*/
void Invoices::ManagementMenu(Bar& bar)
{
	bar.Add(t_("Select Database File"), [=]{myConfig.DBFile = myConfig.SelectDB(); });
	bar.Add(t_("Show Current Database File"), [=]{PromptOK(DeQtf("Current Database file is: \n" + myConfig.DBFile));});
//	bar.Add(t_("Set Company Info"), [=]{if(!setcompanywin.IsOpen()) setcompanywin.Open(this); });
	bar.Add(t_("Select Output Directory"), [=]{ myConfig.OutputDirectory = myConfig.GetOutputDirectory(); });
}

Invoices::Invoices()
{
	CtrlLayout(*this, "Invoices");
	AddFrame(mainmenu);
	mainmenu.Set(THISFN(MainMenu));
	
	// from http://leonardoce.interfree.it/leowiki.html "simple configfile"
	
	if(FileExists(myConfig.configfile))
	{
        VectorMap<String, String> cfg = LoadIniFile(myConfig.configfile);
        myConfig.DBFile = cfg.Get("DBFile", Null);
	}
	else {
		myConfig.GetOutputDirectory();
		myConfig.DBFile = myConfig.SelectDB();
	}
}

// Shortcut keys defined
/*
bool Invoices::Key(dword key, int count)
{
	if(key == K_CTRL_E && !ewin.IsOpen())
		ewin.Open(this);
	else
	if(key == K_CTRL_F && !fwin.IsOpen())
		fwin.Open(this);
	return false;
}
*/

GUI_APP_MAIN
{
	Configs myConfig;
	if(FileExists(myConfig.configfile))
	{
        VectorMap<String, String> cfg = LoadIniFile(myConfig.configfile);
        myConfig.DBFile = cfg.Get("DBFile", Null);
	}
	else {
		myConfig.DBFile = myConfig.SelectDB();
	}
	
	Sqlite3Session sqlite3;
	if(!sqlite3.Open("/home/james/upp/MyApps/Customers/sample.db")) {
		Exclamation("Can't create or open database file\n");
		return;
	}
	
	SQL = sqlite3;
	
	Invoices().Run();
}
