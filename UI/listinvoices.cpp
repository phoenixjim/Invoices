#include "Invoices.h"
#include "listinvoices.h"
#include "../Tables/tables.h"

	ListInvoicesWindow::ListInvoicesWindow() {
	String DBFile;
	String configfile = ConfigFile();
	CtrlLayout(*this, "List Invoices");
	
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
	
	SqlId Invoices("Invoices"), id("id"), invoiceNumber("invoiceNumber"), customerId("customerId"),
		transactionDate("transactionDate"), terms("terms"), nonTaxableSub("nonTaxableSub"), taxableSub("taxableSub"), 
		tax("tax"), grandTotal("grandTotal"), amtPaid("amtPaid"), datePaid("datePaid"), status("status");

	ListInvoicesArray.SetTable(Invoices, id);
		
	// ListInvoicesArray.AddKey(id); See above line
	// ListInvoicesArray.Join(BOOK_ID, book); // joins id from other db to this id
	// ListInvoicesArray.AddColumn(name, "Name");  .SetConvert(DateIntConvert());
	ListInvoicesArray.AddColumn(invoiceNumber,"Invoice#");
	ListInvoicesArray.AddColumn(customerId, "Cust id");
	ListInvoicesArray.AddColumn(transactionDate, "Transaction Date").SetConvert(DateIntConvert());
	ListInvoicesArray.AddColumn(terms, "Terms");
	ListInvoicesArray.AddColumn(nonTaxableSub, "Non-Taxable Sub").SetConvert(ConvDouble());
	ListInvoicesArray.AddColumn(taxableSub, "Taxable Sub").SetConvert(ConvDouble());
	ListInvoicesArray.AddColumn(tax, "Tax").SetConvert(ConvDouble());
	ListInvoicesArray.AddColumn(grandTotal, "Grand Total").SetConvert(ConvDouble());
	ListInvoicesArray.AddColumn(amtPaid, "Amount Paid").SetConvert(ConvDouble());
	ListInvoicesArray.AddColumn(datePaid, "Date Paid").SetConvert(DateIntConvert());
	ListInvoicesArray.AddColumn(status, "Status");
	// ListInvoicesArray.ColumnWidths("5 5 10 12 5 5 5 5 5 5");
	ListInvoicesArray.SetOrderBy(invoiceNumber);
	
	Sql sql(sqlite3);
	sql.Execute("select * from Invoices");


	while(sql.Fetch())
		ListInvoicesArray.Add(sql);
	// ListInvoicesArray.WhenBar = THISBACK(BorrowMenu);
	// ListInvoicesArray.WhenLeftDouble = THISBACK(EditBorrow);
	// ListInvoicesArray.GoEndPostQuery();
	
};

String ListInvoicesWindow::SelectDB()
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
