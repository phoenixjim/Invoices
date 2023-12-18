#include "DBUI.h"
#include "../Utils/configs.h"

InvoicesWindow::InvoicesWindow()
{
	CtrlLayout ( *this, "List Invoices" );
	MinimizeBox();

	btnPrint << [=] { btnPrintClicked(); };
	btnApplyPayment << [=] { btnApplyPaymentClicked(); };
	btnVoid << [=] { btnVoidClicked() ; };
	btnFixDate << [=] { btnFixDateClicked(); };
	btnByPaid << [=] { btnByPaidClicked(); };
	btnByBalanceDue << [=] { btnByBalanceDueClicked(); };
	btnByDates << [=] { btnByDatesClicked(); };
	btnByCustomer << [=] { btnByCustomerClicked(); };
	btnByVoided << [=] { btnByVoidedClicked(); };
	btnAll << [=] { btnAllClicked(); };
	InvoicesArray.SetTable ( INVOICES, INVOICENUMBER ).AllSorting(); // INVOICENUMBER CHANGED FROM INVOICEID

	// InvoicesArray.Join(BOOK_ID, book); // joins id from other db to this id
	InvoicesArray.AddColumn ( INVOICENUMBER, "Invoice#" );
	InvoicesArray.AddColumn ( CUSTOMERID, "Cust id" );
	InvoicesArray.AddColumn ( TRANSACTIONDATE, "Transaction Date" ).SetConvert ( DateIntConvert() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignCenter();
	InvoicesArray.AddColumn ( TERMS, "Terms" );
	InvoicesArray.AddColumn ( NONTAXABLESUB, "Non-Taxable Sub" ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	InvoicesArray.AddColumn ( TAXABLESUB, "Taxable Sub" ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	InvoicesArray.AddColumn ( TAX, "Tax" ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	InvoicesArray.AddColumn ( GRANDTOTAL, "Grand Total" ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	InvoicesArray.AddColumn ( AMTPAID, "Amount Paid" ).SetConvert ( ConvDouble() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	InvoicesArray.AddColumn ( DATEPAID, "Date Paid" ).SetConvert ( DateIntConvert() ).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignCenter();
	InvoicesArray.AddColumn ( STATUS, "Status" );
	InvoicesArray.ColumnWidths("12 15 25 0 25 20 20 20 20 20 10");
	InvoicesArray.Appending().Removing().MultiSelect();

	InvoicesArray.SetOrderBy ( INVOICENUMBER );
	InvoicesArray.WhenLeftDouble = [=] { btnPrintClicked(); };
	InvoicesArray.WhenEnterKey = [=] { btnPrintClicked(); };
		
	ddFixDate.SetConvert ( DateIntConvert() );
	ddRange1.SetConvert ( DateIntConvert() );
	ddRange2.SetConvert ( DateIntConvert() );

	Sql custSql;
	custSql * Select(CUST_ID, CUSTNAME).From(CUSTOMERS);
	while (custSql.Fetch())
	{
		cbCustomers.Add(custSql[CUST_ID], custSql[CUSTNAME]);
	}
	InvoicesArray.Query();

	dv___7.SetInk(TXTCOLOR);
}
void InvoicesWindow::btnPrintClicked()
{
	int thisInvoice;
	String invoiceQTF = "";
	Report myInvoice;
	
	String footer = "[ $$0,0#00000000000000000000000000000000:Default] [ [s0;= [@5;0 Thank you for your business!]&][s0;= [@5;0 " <<
		myConfig.data.companyname << " @$2022; " << myConfig.data.companyaddress << " @$2022; " << myConfig.data.companycity << ", " << myConfig.data.companystate <<
		" "  << myConfig.data.companyzip << " @$2022; " << myConfig.data.companyphone << " @$2022; " << myConfig.data.companyemail << "]]]";
	
	if(InvoicesArray.GetSelectCount() < 1)
		return;

	// int selected = InvoicesArray.GetSelectCount();
	Vector<int> keys = InvoicesArray.GetSelKeys();

	for (int i = 0; i < keys.GetCount(); i++) {
		// selectlist << "Row " << IntStr(i) << " Invoice " << IntStr(keys[i]) << " \n";

		thisInvoice = keys[i];
		if (IsNull(thisInvoice))
			return;
		
		Sql custSQL;
		Sql linesSQL;
		Sql invoiceSQL;
	
		invoiceSQL * SelectAll().From( INVOICES ).Where( INVOICENUMBER == thisInvoice );
		
		String header = "[ {{5000:5000f0;g0; [s0;%% " + myConfig.data.companyname + " ] :: [s0;>%% [@6 "; // Add paid message right aligned if paid
		if ((int)invoiceSQL[STATUS] < 2) // if not Paid In Full
			header <<  "]]}}]";
		else header << "Paid in Full, Thank you!]]}}]";
	
		custSQL * SelectAll().From( CUSTOMERS ).Where( CUST_ID == invoiceSQL[CUSTOMERID]);
		linesSQL * SelectAll().From( LINEITEMS ).Where( INVOICEIDNUMBER == invoiceSQL[INVOICENUMBER]);
		String taxexempt = "";
		if (invoiceSQL[CUSTOMERID] != 18) taxexempt = ( (custSQL[TAXABLE] == 1) ? "" : "Tax exempt form on file, if required");
		else taxexempt = ( invoiceSQL[CUSTOMERID] != 18 ? "" : "Employer");
		
	
		invoiceQTF << "[ [ &][@6 &][ {{4821:95:482:1266:1666:1670f0;g0; [ " << 
			custSQL[CUSTNAME] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ Invoice No.:]:: [> " << invoiceSQL[INVOICENUMBER] << " ]:: [ " << 
			custSQL[ADDRESS] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ Date:]:: [> " << 
			::Format(Date( 1970, 1, 1) + (int)invoiceSQL[TRANSACTIONDATE]) << " ]:: 	[ " << 
			custSQL[CITY] << ", " << custSQL[STATE] << " " << custSQL[ZIP] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ ]:: [> ]:: 	[ " << 
			custSQL[CONTACT] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ ]:: [> ]:: 	[ " << 
			custSQL[EMAIL] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ Terms:]:: [> [+75 " << invoiceSQL[TERMS] << " ]]}}&][ " << taxexempt << "&][> &]";
			
		// Line items:
		invoiceQTF << "[ [ {{729:2603:1666:1666:1666:1670@L|1 [ Item]:: [ Name]:: [> Price]:: [> Quantity]:: [> Taxable]::|1 [> Subtotal]:: [ ]::-3 [ Description]::-2 [ ]::-1 [ ]:: [ ]:: [ ]}}]]&";
		int linenumber = 0;
	
		double nonTaxable = 0.0, taxable = 0.0, salestax = 0.0, grandtotal = 0.0, totalhours = 0.00;
	
		while (linesSQL.Fetch())
		{
			if (linenumber % 2) invoiceQTF << "[ [ {{729:2603:1666:1666:1666:1670@L|1 [ ";
			else invoiceQTF << "[ [ {{729:2603:1666:1666:1666:1670@W|1 [ ";
	
			SQL * Select(TYPENAME).From(TYPES).Where(TYPENUM == (int)(linesSQL[PRODUCTNAME] ) ); // TYPENUM);
			String name = SQL[TYPENAME].ToString();
	
			invoiceQTF << ++linenumber << "]:: [ " << 
				//linesSQL[PRODUCTNAME] << 
				name << "]:: [> " << prnMoney(linesSQL[PRICE]) <<
				"]:: [> " << prnQty(linesSQL[QTY]) << "]:: [> "<< ( linesSQL[ISTAXABLE] ? "T" : "" ) << "]::|1 [> " << 
				prnMoney(linesSQL[TOTAL]) << "]:: [ ]::-3 [ " << linesSQL[DESCRIPTION] << "]::-2 [ ]::-1 [ ]:: [ ]:: [ ]}}]]&";
			if (custSQL[TAXABLE] == 1 && linesSQL[ISTAXABLE] == 1) { // calc sales tax, add totals to taxablesub
				taxable += round((double)linesSQL[PRICE] * (double)linesSQL[QTY], 2);
			}
			else {
				nonTaxable += round((double)linesSQL[PRICE] * (double)linesSQL[QTY], 2);
				if (invoiceSQL[CUSTOMERID] == 18) totalhours += round((double)linesSQL[QTY], 2);
			}
		}
		if (custSQL[TAXABLE] == 1) salestax = round( taxable * ( getTaxRate(custSQL[CUST_ID]) / 100.00 ) , 2 );
		else salestax = 0.00;
		grandtotal = nonTaxable + taxable + salestax;
	
		SQL * SqlUpdate(INVOICES)(TAXABLESUB,taxable)(NONTAXABLESUB,nonTaxable)(TAX, salestax)(GRANDTOTAL,grandtotal).Where(INVOICENUMBER == thisInvoice); // INVOICENUMBER CHANGED FROM INVOICEID
		InvoicesArray.ReQuery();
		
		// Need to recalc values below...
		double amtPaid = (IsNull(invoiceSQL[AMTPAID]) ?  (double)0.00 : (double)invoiceSQL[AMTPAID]);
		if (invoiceSQL[CUSTOMERID] == 18)
			invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [  ]:: [  ]:: [ ]:: [ Total Hours:]::a4/15 [> " << prnQty(totalhours) << "]}}]]&";
		else invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Taxable Sub:]::a4/15 [> " << prnMoney(invoiceSQL[TAXABLESUB]) << "]}}]]&";
		if (invoiceSQL[CUSTOMERID] != 18) {
			invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ NonTaxable Sub:]::a4/15 [> " << prnMoney(invoiceSQL[NONTAXABLESUB]) << "]}}]]&";
			invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Tax:]::a4/15 [> " << prnMoney(invoiceSQL[TAX]) << "]}}]]&";
		}
		invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Total:]::a4/15 [> " << prnMoney(invoiceSQL[GRANDTOTAL]) << "]}}]]&";
		invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Amount Paid:]::a4/15 [> " << prnMoney(invoiceSQL[AMTPAID]) << "]}}]]&";
		invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Balance Due:]::a4/15 [> " << prnMoney((double)invoiceSQL[GRANDTOTAL] - (double)invoiceSQL[AMTPAID]) << "]}}]]&";
		myInvoice.Header(header).Footer(footer);
		myInvoice << invoiceQTF;
		Perform ( myInvoice );
		invoiceQTF = "";
	}
}

void InvoicesWindow::btnApplyPaymentClicked()
{
	if(!InvoicesArray.IsCursor())
		return;
	int thisInvoice = InvoicesArray.GetKey();
	if (IsNull(thisInvoice))
		return;
	int status;
	SQL * SelectAll().From(INVOICES).Where(INVOICENUMBER == thisInvoice); // INVOICENUMBER CHANGED FROM INVOICEID
	SQL.Fetch();
	if (SQL[STATUS] == 0) return;
	if (IsNull(edbPayment)) {
		edbPayment.SetData(SQL[GRANDTOTAL]);
		status = 2;
	}
	else {
		status = (SQL[GRANDTOTAL] <= edbPayment.GetData()) ? 2 : 1;
	}
	SQL * SqlUpdate(INVOICES)(AMTPAID,edbPayment.GetData())(DATEPAID,SQL[TRANSACTIONDATE])(STATUS, status).Where(INVOICENUMBER == thisInvoice); // INVOICENUMBER CHANGED FROM INVOICEID
	InvoicesArray.ReQuery();
	InvoicesArray.FindSetCursor(thisInvoice);
}

void InvoicesWindow::btnVoidClicked()
{
	if(InvoicesArray.GetSelectCount() < 1)
		return;

	int thisInvoice = -1;
	Vector<int> keys = InvoicesArray.GetSelKeys();

	for (int i = 0; i < keys.GetCount(); i++) {

	// int thisInvoice = InvoicesArray.GetKey();
		thisInvoice = keys[i];
	
		if (thisInvoice == -1)
			return;
	
		Sql inv;
		inv * SelectAll().From(INVOICES).Where(INVOICENUMBER == thisInvoice); // INVOICENUMBER CHANGED FROM INVOICEID
		inv.Fetch();
		if ((double)inv[AMTPAID] > 0.0) {
			Exclamation("Can't void after receiving payment");
			return;
		}
		if (PromptOKCancel("Are you sure? This can't be undone...")) {
			long invoice = (int64)inv[INVOICENUMBER];
			inv * SqlUpdate(INVOICES)(STATUS, 0).Where(INVOICENUMBER == thisInvoice); // Void = 0,  // INVOICENUMBER CHANGED FROM INVOICEID
			Sql li;
			li * Delete(LINEITEMS).Where(INVOICEIDNUMBER == (int64)thisInvoice);
			InvoicesArray.ReQuery();
			InvoicesArray.FindSetCursor(thisInvoice);
		}
	}
}

void InvoicesWindow::btnFixDateClicked()
{
	if (IsNull(ddFixDate)) return;
	
	if(!InvoicesArray.IsCursor())
		return;
	int thisInvoice = InvoicesArray.GetKey();
	if (IsNull(thisInvoice))
		return;
	Sql inv;
	inv * SelectAll().From(INVOICES).Where(INVOICENUMBER == thisInvoice); // INVOICENUMBER CHANGED FROM INVOICEID
	inv.Fetch();
	inv[TRANSACTIONDATE] = ddFixDate.GetData();
	if (!IsNull(inv[DATEPAID])) 
		inv * SqlUpdate(INVOICES)(TRANSACTIONDATE, ddFixDate.GetData())(DATEPAID, ddFixDate.GetData()).Where(INVOICENUMBER == thisInvoice); // INVOICENUMBER CHANGED FROM INVOICEID
	else inv * SqlUpdate(INVOICES)(TRANSACTIONDATE, ddFixDate.GetData()).Where(INVOICENUMBER == thisInvoice); // INVOICENUMBER CHANGED FROM INVOICEID
	
	InvoicesArray.ReQuery();
	InvoicesArray.FindSetCursor(thisInvoice);
}


void InvoicesWindow::btnByPaidClicked()
{
	InvoicesArray.ReQuery(STATUS == 2);
}

void InvoicesWindow::btnAllClicked()
{
	InvoicesArray.ReQuery(STATUS < 3);
}

void InvoicesWindow::btnByBalanceDueClicked()
{
	InvoicesArray.ReQuery(STATUS == 1);
}

void InvoicesWindow::btnByDatesClicked()
{
	if ( IsNull ( ddRange1 ) || IsNull ( ddRange2 ))
	{
		return;
	}

	SqlBool where;
	where = Between(DATEPAID, ddRange1.GetData().ToString(), ddRange2.GetData().ToString());
	InvoicesArray.ReQuery(where);
}

void InvoicesWindow::btnByCustomerClicked()
{
	if ( IsNull ( cbCustomers ) )
	{
		return;
	}

	int idNum = 1;
	idNum += cbCustomers.GetIndex();
	if (IsNull(idNum) || idNum < 1)
		return;
	SqlBool where;
	where = CUSTOMERID == idNum && STATUS > 0;
	InvoicesArray.ReQuery(where);
}

void InvoicesWindow::btnByVoidedClicked()
{
	InvoicesArray.ReQuery(STATUS == 0);
}
