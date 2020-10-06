#include "DBUI.h"
#include "../Utils/configs.h"

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
void InvoicesWindow::btnPrintClicked()
{
	int thisInvoice;
	String invoiceQTF;
	Report myInvoice;
	
	// String header = "{{RW 100:100@W [+60< " + myConfig.companyname + ":: ]"; // Add paid message right aligned if paid
	String header = "[ {{5000:5000f0;g0; [s0;%% " << myConfig.companyname << " ] :: [s0;>%% [@6 ";
	String footer = "[ $$0,0#00000000000000000000000000000000:Default] [ [s0;= [@5;0 Thank you for your business!]&][s0;= [@5;0 Company Name Company Address City, State Zip Phone Email]]]";
	
	if(!InvoicesArray.IsCursor())
		return;
	thisInvoice = InvoicesArray.GetKey();
	if (IsNull(thisInvoice))
		return;
	
	Sql custSQL;
	Sql linesSQL;
	Sql invoiceSQL;

	invoiceSQL * SelectAll().From( INVOICES ).Where( INVOICE_ID == thisInvoice );
	if ((int)invoiceSQL[STATUS] < 3)
		header <<  "]]}}]";
	else header << "Paid in Full, Thank you!]]}}]";

	custSQL * SelectAll().From( CUSTOMERS ).Where( CUST_ID == invoiceSQL[CUSTOMERID]);
	linesSQL * SelectAll().From( LINEITEMS ).Where( INVOICEIDNUMBER == invoiceSQL[INVOICENUMBER]);
	// Need date format!
	invoiceQTF = "[ [ {{5000:5000f0;g0; [ ]:: [>@6 &][>@6 ]}}&][@6 &][ {{3621:495:882:1666:1666:1670f0;g0; [ " << 
		custSQL[CUSTNAME] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ Invoice No.:]:: [> " << invoiceSQL[INVOICENUMBER] << " ]:: [ " << 
		custSQL[ADDRESS] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ Date:]:: [> " << 
		::Format(Date( 1970, 1, 1) + (int)invoiceSQL[TRANSACTIONDATE]) << " ]:: 	[ " << 
		custSQL[CITY] << ", " << custSQL[STATE] << " " << custSQL[ZIP] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ ]:: [> ]:: 	[ " << 
		custSQL[CONTACT] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ ]:: [> ]:: 	[ " << 
		custSQL[EMAIL] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ Terms:]:: [> [+75 " << invoiceSQL[TERMS] << " ]]}}&][> &][> &]";
		
	// Line items:
	invoiceQTF << "[ [ {{785:1231:4093:1092:620:747:1432@L [= [2 Item]]:: [ [2 Name]]:: [ [2 Description]]:: [> [2 Price]]:: [> [2 Qty]]:: [ [2 Tax?]]:: [> [2 Subtotal]]";
	int linenumber = 0;

	while (linesSQL.Fetch())
	{
		if (linenumber % 2) invoiceQTF << "::@L ";
		else invoiceQTF << "::@W ";
		invoiceQTF << "[= [2 " << ++linenumber << "]]:: [ [2 " << linesSQL[PRODUCTNAME] << "]]:: [ [2 " << linesSQL[DESCRIPTION] << "]]:: [> [2 " << Format("$%2!nl",linesSQL[PRICE]) <<
			"]]:: [> [2 " << linesSQL[QTY] << "]]:: [ [2 "<< ( linesSQL[ISTAXABLE] ? "T" : "" ) << "]]:: [> [2 " << Format("$%2!nl",linesSQL[TOTAL]) << "]]";
	}
	
	invoiceQTF << "}}]]&";
	// Minor adjustment needed to align dollar column
	invoiceQTF << "[ [ {{2568:3000:3000:1450f0;g0; [ ]:: [ ]:: [> Taxable Sub:]::a3/15 [> " << 
		Format("$%2!nl",invoiceSQL[TAXABLESUB]) << "]::a0/15 [ ]:: [ ]:: [> NonTaxable Sub:]::a3/15 [> " << 
		Format("$%2!nl",invoiceSQL[NONTAXABLESUB]) << "]::a0/15 [ ]:: [ ]:: [> Sales Tax:]::a3/15 [> " << 
		Format("$%2!nl",invoiceSQL[TAX]) << "]::a0/15 [ ]:: [ ]:: [> Invoice Total:]::a3/15 [> " << 
		Format("$%2!nl",invoiceSQL[GRANDTOTAL]) << "]::a0/15 [ ]:: [ ]:: [> Amount Paid:]::a3/15 [> " << 
		Format("$%2!nl",invoiceSQL[AMTPAID]) << "]::a0/15 [ ]:: [ ]:: [> Balance Due:]::a3/15 [> " << 
		Format("$%2!nl",(double)invoiceSQL[GRANDTOTAL] - (double)invoiceSQL[AMTPAID]) << "]}}]]";
		
	myInvoice.Header(header).Footer(footer);
	
	myInvoice << invoiceQTF;
	Perform ( myInvoice );
	
}

void InvoicesWindow::btnApplyPaymentClicked()
{
	if(!InvoicesArray.IsCursor())
		return;
	int thisInvoice = InvoicesArray.GetKey();
	if (IsNull(thisInvoice))
		return;
	if (IsNull(edbPayment)) {
		Exclamation("Must enter amount!");
		return;
	}
	SQL * SelectAll().From(INVOICES).Where(INVOICE_ID == thisInvoice);
	SQL.Fetch();
	int status = (round(SQL[GRANDTOTAL], 2) >= round((double)edbPayment.GetData(), 2)) ? 3 : 2;
	SQL*SqlUpdate(INVOICES)(AMTPAID,round((double)edbPayment.GetData(), 2))(DATEPAID,SQL[TRANSACTIONDATE])(STATUS, status).Where(INVOICE_ID == thisInvoice);
	InvoicesArray.ReQuery();   
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
