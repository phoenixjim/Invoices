#include "Invoices.h"
#include "listlineitems.h"

ListLineItemsWindow::ListLineItemsWindow() {
	String DBFile;
	String configfile = ConfigFile();

	CtrlLayout(*this, "List Line Items");
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

	SqlId tbl_transaction_detail("tbl_transaction_detail"), 
		id("id"), 
		product_name("product_name"),
		description("description"),
		price("price"),
		qty("qty"),
		total("total"),
		invoice_id("invoice_id"), 
		isTaxable("isTaxable");

	ListLineItemsArray.SetTable(tbl_transaction_detail, id);
		
	// ListLineItemsArray.AddKey(id); See above line
	// ListLineItemsArray.Join(BOOK_ID, book); // joins id from other db to this id
	// ListLineItemsArray.AddColumn(name, "Name");  .SetConvert(DateIntConvert());
	ListLineItemsArray.AddColumn(product_name, "Product Name");
	ListLineItemsArray.AddColumn(description, "Description");
	ListLineItemsArray.AddColumn(price, "Price").SetConvert(ConvDouble());
	ListLineItemsArray.AddColumn(qty, "Qty");
	ListLineItemsArray.AddColumn(total, "Total").SetConvert(ConvDouble());
	ListLineItemsArray.AddColumn(invoice_id, "Invoice#");
	ListLineItemsArray.AddColumn(isTaxable, "Taxable?");
	// ListLineItemsArray.ColumnWidths("5 5 10 12 5 5 5 5 5 5");
	ListLineItemsArray.SetOrderBy(id);
	
	Sql sql(sqlite3);
	sql.Execute("select * from tbl_transaction_detail");


	while(sql.Fetch())
		ListLineItemsArray.Add(sql);
	};
