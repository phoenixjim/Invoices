#include "Invoices.h"
#include "products.h"
ProductsWindow::ProductsWindow() {
	String DBFile;
	String configfile = ConfigFile();
	CtrlLayout(*this, "Products");	
	
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
	SqlId tbl_products("tbl_products"), id("id"), name("name"), description("description"), datePurchased("datePurchased"), cost("cost"), invoice_number("invoice_number");

	ProductArray.SetTable(tbl_products);
		
	ProductArray.AddKey(id);
	// ProductArray.Join(BOOK_ID, book); // joins id from other db to this id
	ProductArray.AddColumn(name, "Name"); // .SetConvert(DateIntConvert());
	ProductArray.AddColumn(description, "Description"); // .SetConvert(DateIntConvert());
	ProductArray.AddColumn(datePurchased, "Date Purchased").SetConvert(DateIntConvert());
	ProductArray.AddColumn(cost, "Cost");
	ProductArray.AddColumn(invoice_number, "Invoice#");
	ProductArray.ColumnWidths("100 100 40 30 30");
	ProductArray.SetOrderBy(id);
	
	Sql sql(sqlite3);
	sql.Execute("select * from tbl_products");


	while(sql.Fetch())
		ProductArray.Add(sql);
	// ProductArray.WhenBar = THISBACK(BorrowMenu);
	// ProductArray.WhenLeftDouble = THISBACK(EditBorrow);
	ProductArray.GoEndPostQuery();
	
}