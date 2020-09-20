#include "Invoices.h"
#include "listinvoices.h"

	ListInvoicesWindow::ListInvoicesWindow() {
	String DBFile;
	String configfile = ConfigFile();
	CtrlLayout(*this, "List Invoices");
	if(FileExists(configfile))
	{
        VectorMap<String, String> cfg = LoadIniFile(configfile);
        DBFile = cfg.Get("DBFile", Null);
	}
	else 
	{
		Exclamation("Can't find DB - ListInvoices");
}
	SQL;
	Sqlite3Session sqlite3;
	if(!sqlite3.Open(DBFile)) {
		Exclamation("Can't create or open database file\n");
		return;
	}
	SQL = sqlite3;
	
	SqlId tbl_transactions("tbl_transactions"), id("id"), invoice_number("invoice_number"), customer_id("customer_id"),
		transaction_date("transaction_date"), terms("terms"), nonTaxableSub("nonTaxableSub"), taxableSub("taxableSub"), 
		tax("tax"), grandTotal("grandTotal"), amtPaid("amtPaid"), datePaid("datePaid"), status("status");

	ListInvoicesArray.SetTable(tbl_transactions, id);
		
	// ListInvoicesArray.AddKey(id); See above line
	// ListInvoicesArray.Join(BOOK_ID, book); // joins id from other db to this id
	// ListInvoicesArray.AddColumn(name, "Name");  .SetConvert(DateIntConvert());
	ListInvoicesArray.AddColumn(invoice_number,"Invoice#");
	ListInvoicesArray.AddColumn(customer_id, "Cust id");
	ListInvoicesArray.AddColumn(transaction_date, "Transaction Date").SetConvert(DateIntConvert());
	ListInvoicesArray.AddColumn(terms, "Terms");
	ListInvoicesArray.AddColumn(nonTaxableSub, "Non-Taxable Sub").SetConvert(ConvDouble());
	ListInvoicesArray.AddColumn(taxableSub, "Taxable Sub").SetConvert(ConvDouble());
	ListInvoicesArray.AddColumn(tax, "Tax").SetConvert(ConvDouble());
	ListInvoicesArray.AddColumn(grandTotal, "Grand Total").SetConvert(ConvDouble());
	ListInvoicesArray.AddColumn(amtPaid, "Amount Paid").SetConvert(ConvDouble());
	ListInvoicesArray.AddColumn(datePaid, "Date Paid").SetConvert(DateIntConvert());
	ListInvoicesArray.AddColumn(status, "Status");
	// ListInvoicesArray.ColumnWidths("5 5 10 12 5 5 5 5 5 5");
	ListInvoicesArray.SetOrderBy(invoice_number);
	
	Sql sql(sqlite3);
	sql.Execute("select * from tbl_transactions");


	while(sql.Fetch())
		ListInvoicesArray.Add(sql);
	// ListInvoicesArray.WhenBar = THISBACK(BorrowMenu);
	// ListInvoicesArray.WhenLeftDouble = THISBACK(EditBorrow);
	// ListInvoicesArray.GoEndPostQuery();
};
