#include "DBUI.h"

CreateTimeStatementWindow::CreateTimeStatementWindow()
 { 
 	CtrlLayout(*this, "Create Invoice");
	MinimizeBox(); //
	DropList mytypes;

 	// txtTerms.SetText("Due On Receipt");
 	// txtTaxRate.SetConvert(ConvRate()).SetData(0.00);
 	pInvoice = 0;

	// optProdTaxable.WantFocus(true);
	// btnAdd << [=] { AddItem(); };
	// btnDelete << [=] { ClearItem(); };
	ok << [=] { SaveInvoice(); };
	//btnSubtract << [=] { AdjustPrice(); };
	btnPrintSave << [=] { PrintInvoice(); };
 	cancel << [=] { CancelInvoice(); };
 	// btnUpdate.Hide();
	dblTuesday.SetData(3.00);
	dblThursday.SetData(3.00);
	dblSaturday.SetData(3.00);
	dblMonday.SetData(0.00);
	dblWednesday.SetData(0.00);
	dblFriday.SetData(0.00);
 	SQL * SelectAll().From(CUSTOMERS);
 	while (SQL.Fetch())
 	{
 		cbCustomers.Add(SQL[CUST_ID], SQL[CUSTNAME]);
 	}
 	// customer = SQL[1];
 	cbCustomers.SetData(18);

	SQL.Execute("Select MAX(INVOICENUMBER) From INVOICES");
	SQL.Fetch();
	nextInvoice = (int)SQL[0] + 1; // stol(SQL[0].ToString().ToStd()) + 1;
 	
 	SQL * Select(TYPENUM, TYPENAME).From(TYPES);
	
	while (SQL.Fetch())
	{
		cbProducts.Add(SQL[TYPENUM], SQL[TYPENAME]);
	}
	
 	getHourlyRate();

	SQL.Execute("Select MAX(LINEITEM_ID) From LINEITEMS");
	SQL.Fetch();
	nextLineItem = (int)SQL[0] + 1;
	
	if (nextInvoice < 1) nextInvoice = 1;
	txtInvoice = nextInvoice; // .SetText(IntStr64(nextInvoice));
	cbCustomers.WhenAction << [=] { CustChanged(); };
	// cbProducts.WhenAction << [=] { ProdChanged(); };
	Date now = GetSysDate();
	dtpBillDate.SetConvert(DateIntConvert());
	dtpBillDate.SetText( Format("%", now ).ToString() );

 	totalhours = (double)dblMonday.GetData() +(double)dblTuesday.GetData() +(double)dblWednesday.GetData() +(double)dblThursday.GetData() +(double)dblFriday.GetData() +(double)dblSaturday.GetData();

	lInvNo.SetInk(TXTCOLOR);
	lBillDate.SetInk(TXTCOLOR);
	lName.SetInk(TXTCOLOR);
	lblMonday.SetInk(TXTCOLOR);
	lblTuesday.SetInk(TXTCOLOR);
	lblWednesday.SetInk(TXTCOLOR);
	lblThursday.SetInk(TXTCOLOR);
	lblFriday.SetInk(TXTCOLOR);
	lblSaturday.SetInk(TXTCOLOR);
	lblHourlyRate.SetInk(TXTCOLOR);
	optMarkAsPaid.SetColor(TXTCOLOR);
	bCustDetails.SetInk(TXTCOLOR);
 }
 
void CreateTimeStatementWindow::CustChanged()
{
	int idNum = 1;
	idNum += cbCustomers.GetIndex();
	if (IsNull(idNum) || idNum < 1)
		return;
	// optCustTaxable.Set(SQL % Select(TAXABLE).From(CUSTOMERS).Where(CUST_ID == idNum));
	taxrate = getTaxRate(idNum);
}

void CreateTimeStatementWindow::getHourlyRate()
{
	String txt = "" << SQL % Select(TYPECOST).From(TYPES).Where(TYPENUM == 15);
	dblHourlyRate.SetText(txt);
	hourlyrate = dblHourlyRate.GetData();
}
/*
void CreateTimeStatementWindow::AdjustPrice() // subtract sales tax
{
	if (IsNull(txtPrice)) return;
	int idNum = 1;
	idNum += cbCustomers.GetIndex();
	if (IsNull(idNum) || idNum < 1)
		return;
	double salestax = getTaxRate( idNum ) / 100.00;
	double newPrice = round((double)txtPrice / (double)( 1.00 + salestax ), 2);
	if ((newPrice + salestax ) < txtPrice) newPrice += 0.01;
	txtPrice.SetData( newPrice );
}
*/

void CreateTimeStatementWindow::SaveInvoice()
{
	int idNum = 6;

	if (idNum < 1 || IsNull(cbCustomers) || IsNull(dtpBillDate))
	{
		PromptOK("Are you missing something? (Customer, Date or items)");
		return;
	}
	double nonTaxable = 0.00, taxable = 0.00, salestax = 0.00, grandTotal = 0.00;

	// replace next with workday totals mon - sat
	SQL.Execute("Select MAX(LINEITEM_ID) From LINEITEMS");
	SQL.Fetch();
	nextLineItem = (int)SQL[0] + 1;

	totalhours = (double)dblMonday.GetData() +(double)dblTuesday.GetData() +(double)dblWednesday.GetData() +(double)dblThursday.GetData() +(double)dblFriday.GetData() +(double)dblSaturday.GetData();
	nonTaxable += totalhours * hourlyrate;
	if ((double) dblMonday.GetData() > 0.24) {
		SQL * Insert(LINEITEMS)
		(LINEITEM_ID, (int)nextLineItem++)
		(PRODUCTNAME, 15) // get from table
		(DESCRIPTION, "Monday hours")
		(PRICE, hourlyrate)
		(QTY, (double)dblMonday.GetData())
		(TOTAL, (double)dblMonday.GetData() * hourlyrate )
		(INVOICEIDNUMBER, (int64)txtInvoice)
		(ISTAXABLE, 0);
	}
	if ((double) dblTuesday.GetData() > 0.24) {
		SQL * Insert(LINEITEMS)
		(LINEITEM_ID, (int)nextLineItem++)
		(PRODUCTNAME, 15) // get from table
		(DESCRIPTION, "Tuesday hours")
		(PRICE, hourlyrate)
		(QTY, (double)dblTuesday.GetData())
		(TOTAL, (double)dblTuesday.GetData() * hourlyrate )
		(INVOICEIDNUMBER, (int64)txtInvoice)
		(ISTAXABLE, 0);
	}
	if ((double) dblWednesday.GetData() > 0.24) {
		SQL * Insert(LINEITEMS)
		(LINEITEM_ID, (int)nextLineItem++)
		(PRODUCTNAME, 15) // get from table
		(DESCRIPTION, "Wednesday hours")
		(PRICE, hourlyrate)
		(QTY, (double)dblWednesday.GetData())
		(TOTAL, (double)dblWednesday.GetData() * hourlyrate )
		(INVOICEIDNUMBER, (int64)txtInvoice)
		(ISTAXABLE, 0);
	}
	if ((double) dblThursday.GetData() > 0.24) {
		SQL * Insert(LINEITEMS)
		(LINEITEM_ID, (int)nextLineItem++)
		(PRODUCTNAME, 15) // get from table
		(DESCRIPTION, "Thursday hours")
		(PRICE, hourlyrate)
		(QTY, (double)dblThursday.GetData())
		(TOTAL, (double)dblThursday.GetData() * hourlyrate )
		(INVOICEIDNUMBER, (int64)txtInvoice)
		(ISTAXABLE, 0);
	}
	if ((double) dblFriday.GetData() > 0.24) {
		SQL * Insert(LINEITEMS)
		(LINEITEM_ID, (int)nextLineItem++)
		(PRODUCTNAME, 15) // get from table
		(DESCRIPTION, "Friday hours")
		(PRICE, hourlyrate)
		(QTY, (double)dblFriday.GetData())
		(TOTAL, (double)dblFriday.GetData() * hourlyrate )
		(INVOICEIDNUMBER, (int64)txtInvoice)
		(ISTAXABLE, 0);
	}
	if ((double) dblSaturday.GetData() > 0.24) {
		SQL * Insert(LINEITEMS)
		(LINEITEM_ID, (int)nextLineItem++)
		(PRODUCTNAME, 15) // get from table
		(DESCRIPTION, "Saturday hours")
		(PRICE, hourlyrate)
		(QTY, (double)dblSaturday.GetData())
		(TOTAL, (double)dblSaturday.GetData() * hourlyrate )
		(INVOICEIDNUMBER, (int64)txtInvoice)
		(ISTAXABLE, 0);
	}
	salestax = 0.0;
	
	grandTotal += salestax + nonTaxable + taxable;
	int custId = 1;
	custId += cbCustomers.GetIndex();

	SQL * Insert(INVOICES)
		(INVOICENUMBER, (int64)txtInvoice)
		(CUSTOMERID, custId)
		(TRANSACTIONDATE, dtpBillDate.GetData())
		(TERMS, terms)
		(NONTAXABLESUB, (double)nonTaxable)
		(TAXABLESUB, (double)taxable)
		(TAX, (double)salestax)
		(GRANDTOTAL, (double)grandTotal)
		(AMTPAID, 0.0)
		(STATUS, 1);
	pInvoice = (int64)txtInvoice;
	// ClearItem();
	txtInvoice = StrInt(txtInvoice.GetData().ToString()) + 1;
}

void CreateTimeStatementWindow::MarkAsPaid()
{
	// int thisInvoice = StrInt(txtInvoice.GetData().ToString()) - 1;
	
	if (IsNull(pInvoice) || pInvoice==0)
		{
		Exclamation("No invoice number saved!");
		return;
	}
	int status;
	SQL * SelectAll().From(INVOICES).Where(INVOICENUMBER == pInvoice); // INVOICENUMBER CHANGED FROM INVOICEID
	SQL.Fetch();
	// edbPayment.SetData(SQL[GRANDTOTAL]);
	status = 2;
	
	SQL * SqlUpdate(INVOICES)(AMTPAID,SQL[GRANDTOTAL])(DATEPAID,SQL[TRANSACTIONDATE])(STATUS, status).Where(INVOICENUMBER == pInvoice); // CHANGED FROM INVOICEID
}

void CreateTimeStatementWindow::PrintInvoice()
{
	String invoiceQTF;
	Report myInvoice;
	
	SaveInvoice();
	if (optMarkAsPaid.GetData() == 1)  MarkAsPaid();
	// pInvoice = InvoicesArray.GetKey();
	if (pInvoice == 0 || IsNull(pInvoice) || totalhours < 0.24)
	{
		Exclamation("Invalid - returning.");
		return;
	}
	
	Sql custSQL;
	Sql linesSQL;
	Sql invoiceSQL;

	String header = "[ {{5000:5000f0;g0; [s0;%% " + myConfig.data.companyname + " ] :: [s0;>%% [@6 "; // Add paid message right aligned if paid
	String footer = "[ $$0,0#00000000000000000000000000000000:Default] [ [s0;= [@5;0 Thank you for your business!]&][s0;= [@5;0 " <<
		myConfig.data.companyname << " @$2022; " << myConfig.data.companyaddress << " @$2022; " << myConfig.data.companycity << ", " << myConfig.data.companystate <<
		" "  << myConfig.data.companyzip << " @$2022; " << myConfig.data.companyphone << " @$2022; " << myConfig.data.companyemail << "]]]";
	
	invoiceSQL * SelectAll().From( INVOICES ).Where( INVOICENUMBER == pInvoice ); //CHANGED FROM INVOICEID
	
	if ((int)invoiceSQL[STATUS] < 2)
		header <<  "]]}}]";
	else header << "Paid in Full, Thank you!]]}}]";
	custSQL * SelectAll().From( CUSTOMERS ).Where( CUST_ID == invoiceSQL[CUSTOMERID]);
	String taxexempt = ((custSQL[TAXABLE] == 1) ? "" : "Tax exempt form on file");
	// Need date format!
	invoiceQTF = "[ [ &][@6 &][ {{4821:95:482:1266:1666:1670f0;g0; [ " << 
		custSQL[CUSTNAME] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ Invoice No.:]:: [> " << invoiceSQL[INVOICENUMBER] << " ]:: [ " << 
		custSQL[ADDRESS] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ Date:]:: [> " <<  ::Format(Date( 1970, 1, 1) + (int)invoiceSQL[TRANSACTIONDATE]) <<" ]:: 	[ " << 
		custSQL[CITY] << ", " << custSQL[STATE] << " " << custSQL[ZIP] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ ]:: [> ]:: 	[ " << 
		custSQL[CONTACT] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ ]:: [> ]:: 	[ " << 
		custSQL[EMAIL] << " ]:: [@6 ]:: [@6 ]:: [@6 ]:: [ Terms:]:: [> [+75 " << invoiceSQL[TERMS] << " ]]}}&][ " << taxexempt << "&][> &]";
		
	// Line items:
	invoiceQTF << "[ [ {{729:2603:1666:1666:1666:1670@L|1 [ Item]:: [ Name]:: [> Price]:: [> Quantity]:: [> Taxable]::|1 [> Subtotal]:: [ ]::-3 [ Description]::-2 [ ]::-1 [ ]:: [ ]:: [ ]}}]]&";
	int linenumber = 0;

	linesSQL * SelectAll().From( LINEITEMS ).Where( INVOICEIDNUMBER == invoiceSQL[INVOICENUMBER]);

	while (linesSQL.Fetch())
	{
		if (linenumber % 2) invoiceQTF << "[ [ {{729:2603:1666:1666:1666:1670@L|1 [ ";
		else invoiceQTF << "[ [ {{729:2603:1666:1666:1666:1670@W|1 [ ";
		
		SQL * Select(TYPENAME).From(TYPES).Where(TYPENUM == (int)(linesSQL[PRODUCTNAME] ) ); // TYPENUM);
		String name = SQL[TYPENAME].ToString();
		
		invoiceQTF << ++linenumber << "]:: [ " << 
			// linesSQL[PRODUCTNAME] << 
			name << "]:: [> " << prnMoney(linesSQL[PRICE]) <<
			"]:: [> " << prnQty(linesSQL[QTY]) << "]:: [> "<< ( linesSQL[ISTAXABLE] ? "T" : "" ) << "]::|1 [> " << 
			prnMoney(linesSQL[TOTAL]) << "]:: [ ]::-3 [ " << linesSQL[DESCRIPTION] << "]::-2 [ ]::-1 [ ]:: [ ]:: [ ]}}]]&";
	}
	
	// Minor adjustment needed to align dollar column
	double amtPaid = (IsNull(invoiceSQL[AMTPAID]) ?  (double)0.00 : (double)invoiceSQL[AMTPAID]);
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [  ]:: [  ]:: [ ]:: [ Total Hours:]::a4/15 [> " << prnQty(totalhours) << "]}}]]&";
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ NonTaxable Sub:]::a4/15 [> " << prnMoney(invoiceSQL[NONTAXABLESUB]) << "]}}]]&";
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Tax:]::a4/15 [> " << prnMoney(invoiceSQL[TAX]) << "]}}]]&";
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Total:]::a4/15 [> " << prnMoney(invoiceSQL[GRANDTOTAL]) << "]}}]]&";
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Amount Paid:]::a4/15 [> " << prnMoney(amtPaid) << "]}}]]&";
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Balance Due:]::a4/15 [> " << prnMoney((double)invoiceSQL[GRANDTOTAL] - (double)amtPaid) << "]}}]]&";

	myInvoice.Header(header).Footer(footer);
	
	myInvoice << invoiceQTF;
	Perform ( myInvoice );
	Close();
}

void CreateTimeStatementWindow::CancelInvoice()
{
//	ClearItem();
	Close();
}
/*
void CreateTimeStatementWindow::AddItem()
{
	int idNum = cbProducts.GetIndex() + 1;
	if (IsNull(idNum) || IsNull(txtDescription) || IsNull(txtPrice) || IsNull(txtQty))
		return;
	// SQL * Select(TYPENAME).From(TYPES).Where(idNum == TYPENUM);
	// String name = SQL[TYPENAME];
	arrayLineItems.Add( cbProducts.GetData(),
		// name,
		txtDescription.GetText().ToString(),
		StrDbl(txtPrice.GetText().ToString()),
		StrDbl(txtQty.GetText().ToString()), 
		optProdTaxable.Get(),
		StrDbl(txtPrice.GetText().ToString()) * StrDbl(txtQty.GetText().ToString()));
	CalcInvoiceTotal();
}

void CreateTimeStatementWindow::ClearItem()
{
	txtDescription.SetText("");
	txtPrice.SetText("");
	txtQty.SetText("");
	cbProducts.SetIndex(0);
	cbCustomers.SetIndex(17);
	optProdTaxable.Set(0);
	arrayLineItems.Clear();
	CalcInvoiceTotal();
}

double CreateTimeStatementWindow::CalcItemTotal(int itemnumber)
{
	if (IsNull(arrayLineItems.GetColumn(itemnumber, StrDbl(~PRICE))) || IsNull(arrayLineItems.GetColumn(itemnumber, StrDbl(~QTY))))
		return 0.0;
	return round((double)arrayLineItems.GetColumn(itemnumber, StrDbl(~PRICE)) * (double)arrayLineItems.GetColumn(itemnumber, StrDbl(~QTY)), 2);
}
*/
void CreateTimeStatementWindow::CalcInvoiceTotal()
{
	double nonTaxable = 0.0, taxable = 0.0, salestax = 0.0, grandtotal = 0.0, totalhours = 0.00;
	totalhours = (double)dblMonday.GetData() +(double)dblTuesday.GetData() +(double)dblWednesday.GetData() +(double)dblThursday.GetData() +(double)dblFriday.GetData() +(double)dblSaturday.GetData();
	if (totalhours < 0.25 ) return;
	nonTaxable += totalhours * hourlyrate;
	// ERROR in SalesTax Calculation!!!!
	salestax = 0.00;
	
	grandtotal = nonTaxable + taxable + salestax;
}

/*
void CreateTimeStatementWindow::DeleteRow()
{
	int id = arrayLineItems.GetKey();
	if(IsNull(id))
		return;

	arrayLineItems.DoRemove();
	CalcInvoiceTotal();
}
*/	