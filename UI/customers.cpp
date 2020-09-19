#include "Invoices.h"
#include "customers.h"

#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
#include "Sql/sch_source.h"


CustomersWindow::CustomersWindow() {
	String DBFile;
	String configfile = ConfigFile();
	CtrlLayout(*this, "Customers");
	
	if(FileExists(configfile))
	{
        VectorMap<String, String> cfg = LoadIniFile(configfile);
        DBFile = cfg.Get("DBFile", Null);
	}
	else 
	{
		Exclamation("Can't find DB");
	}
	SQL;
	Sqlite3Session sqlite3;
	if(!sqlite3.Open(DBFile)) {
		Exclamation("Can't create or open database file\n");
		return;
	}
	SQL = sqlite3;
	SqlId tbl_customers("tbl_customers"), id("id"), name("name"), email("email"), contact("contact"), address("address"), city("city"), state("state"), zip("zip"), taxable("taxable");

	CustArray.SetTable(tbl_customers);
		
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
	sql.Execute("select * from tbl_customers");


	while(sql.Fetch())
		CustArray.Add(sql);
	// CustArray.WhenBar = THISBACK(BorrowMenu);
	// CustArray.WhenLeftDouble = THISBACK(EditBorrow);
	CustArray.GoEndPostQuery();
}