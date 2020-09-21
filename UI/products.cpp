#include "Invoices.h"
#include "products.h"
#include "../Tables/tables.h"

ProductsWindow::ProductsWindow() {
	CtrlLayout(*this, "Products");	

	btnAddProduct << [=] { btnAddProductClick(); };
	
	btnUpdateProduct << [=] { btnUpdateProductClick(); };
	
	btnShowAllProduct << [=] { btnShowAllProductClick(); };
	
	btnProductRange << [=] { btnProductRangeClick(); };
		
	if(FileExists(configfile))
	{
        VectorMap<String, String> cfg = LoadIniFile(configfile);
        DBFile = cfg.Get("DBFile", Null);
	}
	else 
	{
		SelectDB();
	 } 
	
	SQL;
	Sqlite3Session sqlite3;
	if(!sqlite3.Open(DBFile)) {
		Exclamation("Can't create or open database file\n");
		return;
	}
	
	SQL = sqlite3;
	SqlId Products("Products"), id("id"), name("name"), description("description"), datePurchased("datePurchased"), cost("cost"), invoiceNumber("invoiceNumber");

	ProductArray.SetTable(Products);
		
	ProductArray.AddKey(id);
	// ProductArray.Join(BOOK_ID, book); // joins id from other db to this id
	ProductArray.AddColumn(name, "Name");
	ProductArray.AddColumn(description, "Description");
	ProductArray.AddColumn(datePurchased, "Date Purchased").SetConvert(DateIntConvert());
	ProductArray.AddColumn(cost, "Cost");
	ProductArray.AddColumn(invoiceNumber, "Invoice#");
	ProductArray.ColumnWidths("100 100 40 30 30");
	ProductArray.SetOrderBy(id);
	
	Sql sql(sqlite3);
	sql.Execute("select * from Products");

	while(sql.Fetch())
		ProductArray.Add(sql);

	// ProductArray.WhenBar = THISBACK(BorrowMenu);
	ProductArray.WhenLeftDouble = [=] { EditRow();};
	// ProductArray.GoEndPostQuery();
}

void ProductsWindow::btnAddProductClick()
{

	PromptOK(__func__);
}

void ProductsWindow::btnUpdateProductClick()
{
	PromptOK(__func__);
}

void ProductsWindow::btnShowAllProductClick()
{
	PromptOK(__func__);
}

void ProductsWindow::btnProductRangeClick()
{
	PromptOK(__func__);
}

void ProductsWindow::EditRow()
{
	PromptOK(__func__);
}

String ProductsWindow::SelectDB()
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
/*
String ProductsWindow::GetOutputDirectory()
{
	if ( !selectodirwin.ExecuteSelectDir ( t_ ( "Choose output directory" ) ) )
	{
		return "";
	}

	cfg << "OutputDirectory=" << selectodirwin.Get() << "\n";

	if ( !SaveFile ( ConfigFile(), cfg ) )
	{
		Exclamation ( "Error saving configuration!" );
	}
	return selectodirwin.Get();
}
*/