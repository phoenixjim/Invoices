#include "Invoices.h"
#include "customers.h"
#include "../Tables/tables.h"

#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
#include "Sql/sch_source.h"


CustomersWindow::CustomersWindow() {
	CtrlLayout(*this, "Customers");
	btnAddCustomer << [=] { btnAddCustomerClick(); }; // THISBACK is not needed in c++11 world and could be replaced with lambda.
	
	btnSearchCustomer << [=] { btnSearchCustomerClick(); }; // assisted by forum user Klugier
	
	btnUpdateCustomer << [=] { btnUpdateCustomerClick(); };
	
	btnFake << [=] { FakeStub(); };
	
	btnFake.IsVisible() << false;
		
	if(FileExists(configfile))
	{
        VectorMap<String, String> cfg = LoadIniFile(configfile);
        DBFile = cfg.Get("DBFile", Null);
	}
	else {
		DBFile = SelectDB();
	}
	SQL;
	Sqlite3Session sqlite3;
	if(!sqlite3.Open(DBFile)) {
		Exclamation("Can't create or open database file\n");
		return;
	}
	SQL = sqlite3;
	SqlId Customers("Customers"), id("id"), name("name"), email("email"), contact("contact"), address("address"), city("city"), state("state"), zip("zip"), taxable("taxable");
	
	CustArray.SetTable(Customers);
		
	CustArray.AddKey(id);
	// CustArray.Join(BOOK_ID, book); // joins id from other db to this id
	CustArray.AddColumn(name, "Name"); // .SetConvert(DateIntConvert());
	CustArray.AddColumn(email, "Email"); // .SetConvert(DateIntConvert());
	CustArray.AddColumn(contact, "Phone");
	CustArray.AddColumn(address, "Address");
	CustArray.AddColumn(city, "City");
	CustArray.AddColumn(state, "State");
	CustArray.AddColumn(zip, "Zip");
	CustArray.AddColumn(taxable, "Taxable?");
	CustArray.ColumnWidths("40 40 20 50 20 15 10 5");
	CustArray.SetOrderBy(id);
	
	Sql sql(sqlite3);
	sql.Execute("select * from Customers");
	

	while(sql.Fetch())
		CustArray.Add(sql);
	// CustArray.WhenBar
	CustArray.WhenLeftDouble = [=] { EditRow(); };
	
	CustArray.GoEndPostQuery();
}

void CustomersWindow::btnAddCustomerClick()
{

	PromptOK(__func__);
}

void CustomersWindow::btnUpdateCustomerClick()
{
	PromptOK(__func__);
}

void CustomersWindow::btnSearchCustomerClick()
{
	PromptOK(__func__);
}

void CustomersWindow::EditRow()
{
	PromptOK(__func__);
}

void CustomersWindow::FakeStub()
{
}

String CustomersWindow::SelectDB()
{
	selectdbwin.Type(t_("Invoices DB"), "*.jts");
	if(!selectdbwin.ExecuteOpen(t_("Select DB File")))
	{
        return "";
	}
	/*
	from http://leonardoce.interfree.it/leowiki.html "simple configfile"
	*/
	cfg << "DBFile=" << selectdbwin.Get() << "\n";
	if(!SaveFile(ConfigFile(), cfg))
	{
    	Exclamation("Error saving configuration!");
	}
	return selectdbwin.Get();
}
