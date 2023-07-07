#include "DBUI.h"

#define MODEL "Invoices/Tables/Invoices.sch"
#include <Sql/sch_schema.h>
#undef MODEL
void Invoices::MainMenu(Bar& bar)
{
	bar.Sub(t_("File"), [=] (Bar& bar ) {
		FileMenu( bar ); });
	bar.Sub(t_("Reports"), [=] (Bar& bar) {
		ReportsMenu( bar );; });
	bar.Sub(t_("Management"), [=] ( Bar& bar ) {
		ManagementMenu( bar ); });
	bar.Add(t_("About"), [=]{ 
		String about = "[ [ {{10000~ [ Invoices and Reports version " << version << " ] & [ Copyright © 2022 James Lefavour ]&";
		about << " :: [ This software is licensed under the BSD-3-Clause license. Please see the file LICENSE, or visit ]& [^https`:`/`/spdx`.org`/licenses`/BSD-3-Clause`.html^ https://spdx.org/licenses/BSD-3-Clause.html for details.]";
		about << " :: [ Developed using the U`+`+ Library, with help from the forum members there ]:: }}&][ {{3333:6667 [ U`+`+ home page]:: [ [^http`:`/`/www`.ultimatepp`.org`/^ https://www.ultimatepp.org]]:: [ U`+`+ forum]:: [ [^https`:`/`/www`.ultimatepp`.org`/forums^ https://www.ultimatepp.org/forums]]}}]]&";
		about << " For more info including licensing of U`+`+, or it's plugins etc, please visit the website above.";

		PromptOK(about);
	});
	bar.Add(t_("Exit"), [=] {
		Close(); });
}

void Invoices::FileMenu(Bar& bar)
{
	bar.Add(t_("New Invoice"), [=]{ if(!createinvoicewin.IsOpen()) {
		createinvoicewin.ActiveFocus(createinvoicewin.cbCustomers);
		createinvoicewin.Open(this);
		}
	});
	bar.Add(t_("List Invoices"), [=]{
		if(!invoiceswin.IsOpen())	{
			invoiceswin.InvoicesArray.ReQuery();
			invoiceswin.InvoicesArray.GoBegin();
			invoiceswin.Open(this);
			}
		});
	bar.Add(t_("List Line Items"), [=]{
		if(!listlineitemswin.IsOpen())	{
			listlineitemswin.LineItemsArray.ReQuery();
			listlineitemswin.LineItemsArray.GoEnd();
			listlineitemswin.Open(this);
			}
		});
	bar.Add(t_("List Customers"),  [=]{
		if(!custwin.IsOpen()) {
			custwin.CustArray.ReQuery();
		 	custwin.Open(this); 
			}
		});

    bar.Add(t_("List Products"), [=]{ 
    	if(!prodwin.IsOpen()) {
    		prodwin.ProductArray.ReQuery();
    		prodwin.Open(this); 
    		}
    	});
	
    bar.Add(t_("List Types"), [=]{ 
    	if(!typewin.IsOpen()) {
    		typewin.TypeArray.ReQuery();
    		typewin.Open(this); 
    		}
    	});
}

void Invoices::ReportsMenu(Bar& bar)
{
	bar.Add(t_("Income / Sales Tax"), [=]{ if(!taxreportwin.IsOpen()) taxreportwin.Open(this); });
	bar.Add(t_("Income by Customer"), [=]{ if(!incomewin.IsOpen()) incomewin.Open(this); });
	bar.Add(t_("Year by Year compare"), [=]{ if(!year2yearwin.IsOpen()) year2yearwin.Open(this); });
	bar.Add(t_("Profit & Loss Statement"), [=] { if (!pandlwin.IsOpen()) pandlwin.Open(this); });
}

void Invoices::ManagementMenu(Bar& bar)
{
	bar.Add(t_("Select Database File"), [=]{ myConfig.SelectDB(); });
	bar.Add(t_("Show Current Database File"), [=]{ PromptOK(DeQtf("Current Database file is: \n" + myConfig.data.dbfile));});
	bar.Add(t_("Set Company Info"), [=]{if( !setcompanywin.IsOpen()) setcompanywin.Open(this); });
}

Invoices::Invoices()
{
	CtrlLayout(*this, "Invoices");
	MinimizeBox(); // 	

	AddFrame(mainmenu);
	mainmenu.Set( [=] ( Bar& bar ) {
		MainMenu( bar ); });
	Date min = Date(2010,1,1);
	Date max = Date(2040,1,1);
	ConvertDate::SetDefaultMinMax(min, max);
}

GUI_APP_MAIN
{
	Configs myConfig;
	
	int unpaid = 0;
	String notpaid = "";

	Sqlite3Session sqlite3;
	if(!sqlite3.Open(myConfig.data.dbfile)) {
		Exclamation("Can't create or open database file\n");
		return;
	}

	SQL = sqlite3;
	SQL * Select(INVOICENUMBER).From(INVOICES).Where(STATUS==1);
	while(SQL.Fetch()) {
		unpaid++;
		notpaid << "Invoice number " << SQL[INVOICENUMBER] << "\n";
	}
	if (unpaid > 0)
	{
		notpaid << "You have " << IntStr( unpaid ) << " unpaid invoices!";
		PromptOK(DeQtf(notpaid));
	}
	
	Invoices().Run();
}
