#include "DBUI.h"
#include "../configs.h"

// ListInvoices:
InvoicesWindow::InvoicesWindow()
{
	CtrlLayout ( *this, "List Invoices" );

	btnEdit << [=] { btnEditClicked(); };
	btnPrint << [=] { btnPrintClicked(); };
	btnApplyPayment << [=] { btnApplyPaymentClicked(); };
	btnVoid << [=] { btnVoidClicked() ; };
	btnPaidInFull << [=] { btnPaidInFullClicked(); };
	btnFixDate << [=] { btnFixDateClicked(); };
	btnByPaid << [=] { btnByPaidClicked(); };
	btnByBalanceDue << [=] { btnByBalanceDueClicked(); };
	btnByDates << [=] { btnByDatesClicked(); };
	btnByCustomer << [=] { btnByCustomerClicked(); };

	InvoicesArray.SetTable ( INVOICES, INVOICE_ID );

	// InvoicesArray.Join(BOOK_ID, book); // joins id from other db to this id
	InvoicesArray.AddColumn ( INVOICENUMBER, "Invoice#" );
	InvoicesArray.AddColumn ( CUSTOMERID, "Cust id" );
	InvoicesArray.AddColumn ( TRANSACTIONDATE, "Transaction Date" ).SetConvert ( DateIntConvert() );
	InvoicesArray.AddColumn ( TERMS, "Terms" );
	InvoicesArray.AddColumn ( NONTAXABLESUB, "Non-Taxable Sub" ).SetConvert ( ConvDouble() );
	InvoicesArray.AddColumn ( TAXABLESUB, "Taxable Sub" ).SetConvert ( ConvDouble() );
	InvoicesArray.AddColumn ( TAX, "Tax" ).SetConvert ( ConvDouble() );
	InvoicesArray.AddColumn ( GRANDTOTAL, "Grand Total" ).SetConvert ( ConvDouble() );
	InvoicesArray.AddColumn ( AMTPAID, "Amount Paid" ).SetConvert ( ConvDouble() );
	InvoicesArray.AddColumn ( DATEPAID, "Date Paid" ).SetConvert ( DateIntConvert() );
	InvoicesArray.AddColumn ( STATUS, "Status" );
	// InvoicesArray.ColumnWidths("5 5 10 12 5 5 5 5 5 5");
	InvoicesArray.SetOrderBy ( Descending(INVOICENUMBER) );

	// InvoicesArray.WhenLeftClick << [=] { selectedRow = InvoicesArray.GetClickRow(); } ;
	InvoicesArray.Query();

	// InvoicesArray.WhenLeftDouble = THISBACK(EditBorrow);
	// InvoicesArray.GoEndPostQuery();
	
}
/*
void InvoicesWindow::Open(Ctrl* owner)
{
	InvoicesArray.ReQuery();
	InvoicesArray.GoBegin();
	
	TopWindow::Open(owner);
}
*/

void InvoicesWindow::btnPrintClicked()
{
	PromptOK ( __func__ );
}

void InvoicesWindow::btnApplyPaymentClicked()
{
	PromptOK ( __func__ );
}

void InvoicesWindow::btnEditClicked()

{
	PromptOK ( __func__ );
}

void InvoicesWindow::btnVoidClicked()
{
	PromptOK ( __func__ );
}

void InvoicesWindow::btnPaidInFullClicked()
{
	PromptOK ( __func__ );
}

void InvoicesWindow::btnFixDateClicked()
{
	PromptOK ( __func__ );
}

void InvoicesWindow::btnByPaidClicked()
{
	PromptOK ( __func__ );
}

void InvoicesWindow::btnByBalanceDueClicked()
{
	PromptOK ( __func__ );
}

void InvoicesWindow::btnByDatesClicked()
{
	PromptOK ( __func__ );
}

void InvoicesWindow::btnByCustomerClicked()
{
	PromptOK ( __func__ );
}

void InvoicesWindow::btnByVoidedClicked()
{
	PromptOK ( __func__ );
}
