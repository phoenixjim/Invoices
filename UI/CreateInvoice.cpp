#include "DBUI.h"

CreateInvoiceWindow::CreateInvoiceWindow()
 { 
 	CtrlLayout(*this, "Create Invoice");
	MinimizeBox(); //
	DropList mytypes;

 	txtTerms.SetText("Due On Receipt");
 	txtTaxRate.SetConvert(ConvRate()).SetData(0.00);
 	pInvoice = 0;
	optProdTaxable.WantFocus(true);
	btnAdd << [=] { AddItem(); };
	btnDelete << [=] { ClearItem(); };
	ok << [=] { SaveInvoice(); };
	btnSubtract << [=] { AdjustPrice(); };
	btnPrintSave << [=] { PrintInvoice(); };
 	cancel << [=] { CancelInvoice(); };
 	btnUpdate.Hide();
 	
 	arrayLineItems.AddColumn(PRODUCTNAME, "Name", 40).SetConvert(Single<Lookup(TYPES,TYPENUM,TYPENAME)>()).Edit(mytypes);
 	arrayLineItems.AddColumn(DESCRIPTION, "Description", 80);
 	arrayLineItems.AddColumn(PRICE, "Price", 20).SetConvert(ConvCurrency()).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight(); //.SetConvert(ConvDouble()).
 	arrayLineItems.AddColumn(QTY, "Qty", 15);
 	arrayLineItems.AddColumn(ISTAXABLE, "Tax?", 15);
 	arrayLineItems.AddColumn(TOTAL, "Total", 20).SetConvert(ConvCurrency()).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight(); //.SetConvert(ConvDouble())
 	arrayLineItems.Appending() .Removing();
 
 	SQL * Select(CUST_ID, CUSTNAME).From(CUSTOMERS);
 	while (SQL.Fetch())
 	{
 		cbCustomers.Add(SQL[CUST_ID], SQL[CUSTNAME]);
 	}
	
	SQL * Select(TYPENUM, TYPENAME).From(TYPES);
	
	while (SQL.Fetch())
	{
		cbProducts.Add(SQL[TYPENUM], SQL[TYPENAME]);
	}
	
	SQL.Execute("Select MAX(INVOICENUMBER) From INVOICES");
	SQL.Fetch();
	
	nextInvoice = (int)SQL[0] + 1; // stol(SQL[0].ToString().ToStd()) + 1;
	if (nextInvoice < 1) nextInvoice = 1;
	txtInvoice = nextInvoice; // .SetText(IntStr64(nextInvoice));
	cbCustomers.WhenAction << [=] { CustChanged(); };
	cbProducts.WhenAction << [=] { ProdChanged(); };
	Date now = GetSysDate();
	dtpBillDate.SetConvert(DateIntConvert());
	dtpBillDate.SetText( Format("%", now ).ToString() );

	lDesc.SetInk(TXTCOLOR);
	lNTSub.SetInk(TXTCOLOR);
	lTSub.SetInk(TXTCOLOR);
	lTax.SetInk(TXTCOLOR);
	lTotal.SetInk(TXTCOLOR);
	lPrice.SetInk(TXTCOLOR);
	lQty.SetInk(TXTCOLOR);
	lLIName.SetInk(TXTCOLOR);
	bCustDetails.SetInk(TXTCOLOR);
	lInvNo.SetInk(TXTCOLOR);
	lBillDate.SetInk(TXTCOLOR);
	lRate.SetInk(TXTCOLOR);
	lTerms.SetInk(TXTCOLOR);
	lName.SetInk(TXTCOLOR);
	bLineItem.SetInk(TXTCOLOR);
	bLineItems.SetInk(TXTCOLOR);
	bTotals.SetInk(TXTCOLOR);
	lInst.SetInk(TXTCOLOR);
 }
 
void CreateInvoiceWindow::CustChanged()
{
	int idNum = 1;
	idNum += cbCustomers.GetIndex();
	if (IsNull(idNum) || idNum < 1)
		return;
	optCustTaxable.Set(SQL % Select(TAXABLE).From(CUSTOMERS).Where(CUST_ID == idNum));
	txtTaxRate.SetData(getTaxRate(idNum));
}

void CreateInvoiceWindow::ProdChanged()
{
	int idNum = cbProducts.GetIndex() + 1;
	if (IsNull(idNum) || idNum < 1)
		return;
	String txt = "" << SQL % Select(TYPEDESCR).From(TYPES).Where(TYPENUM == idNum);
	txtDescription.SetText(txt);
	double price = SQL % Select(TYPECOST).From(TYPES).Where(TYPENUM == idNum);
	txtPrice.SetText(DblStr(price));
	double quant = SQL % Select(TYPEQUANT).From(TYPES).Where(TYPENUM == idNum);
	txtQty.SetText(DblStr(quant));
	optProdTaxable.Set(SQL % Select(TYPETAXABLE).From(TYPES).Where(TYPENUM == idNum));
}

void CreateInvoiceWindow::AdjustPrice() // subtract sales tax
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

void CreateInvoiceWindow::SaveInvoice()
{
	int idNum = arrayLineItems.GetCount();

	if (idNum < 1 || IsNull(cbCustomers) || IsNull(dtpBillDate))
	{
		PromptOK("Are you missing something? (Customer, Date or items)");
		return;
	}
	double nonTaxable = 0.00, taxable = 0.00, salestax = 0.00, grandTotal = 0.00;

	for (int i = 0; i < idNum; i++)
	{
		if (optCustTaxable.Get() == true && arrayLineItems.Get(i, ISTAXABLE)  == 1) {
			taxable += ((double)arrayLineItems.Get(i, PRICE) ) * (double)arrayLineItems.Get(i, QTY);
		}
		else 	nonTaxable += (double)arrayLineItems.Get(i, PRICE) * (double)arrayLineItems.Get(i, QTY) ;
		
		SQL * Insert(LINEITEMS)
		(PRODUCTNAME, arrayLineItems.Get(i,PRODUCTNAME))
		(DESCRIPTION, arrayLineItems.Get(i, DESCRIPTION))
		(PRICE, (double)arrayLineItems.Get(i, PRICE))
		(QTY, (double)arrayLineItems.Get(i, QTY))
		(TOTAL, (double)arrayLineItems.Get(i, TOTAL))
		(INVOICEIDNUMBER, (int64)txtInvoice)
		(ISTAXABLE, (int)arrayLineItems.Get(i, ISTAXABLE));
	}
	if (optCustTaxable.Get() == true) salestax = (double)round(taxable * (double)txtTaxRate, 2);
	else salestax = 0.0;
	
	grandTotal += salestax + nonTaxable + taxable;
	int custId = 1;
	custId += cbCustomers.GetIndex();

	SQL * Insert(INVOICES)
		(INVOICENUMBER, (int64)txtInvoice)
		(CUSTOMERID, custId)
		(TRANSACTIONDATE, dtpBillDate.GetData())
		(TERMS, ~txtTerms)
		(NONTAXABLESUB, (double)nonTaxable)
		(TAXABLESUB, (double)taxable)
		(TAX, (double)salestax)
		(GRANDTOTAL, (double)grandTotal)
		(AMTPAID, 0.0)
		(STATUS, 1);
	pInvoice = (int64)txtInvoice;
	ClearItem();
	txtInvoice = StrInt(txtInvoice.GetData().ToString()) + 1;
}

void CreateInvoiceWindow::MarkAsPaid()
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

void CreateInvoiceWindow::PrintInvoice()
{
	String invoiceQTF;
	Report myInvoice;
	
	SaveInvoice();
	MarkAsPaid();
	// pInvoice = InvoicesArray.GetKey();
	if (pInvoice == 0 || IsNull(pInvoice))
	{
		Exclamation("No invoice number saved!");
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
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Taxable Sub:]::a4/15 [> " << prnMoney(invoiceSQL[TAXABLESUB]) << "]}}]]&";
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

void CreateInvoiceWindow::CancelInvoice()
{
	ClearItem();
	Close();
}

void CreateInvoiceWindow::AddItem()
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

void CreateInvoiceWindow::ClearItem()
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

double CreateInvoiceWindow::CalcItemTotal(int itemnumber)
{
	if (IsNull(arrayLineItems.GetColumn(itemnumber, StrDbl(~PRICE))) || IsNull(arrayLineItems.GetColumn(itemnumber, StrDbl(~QTY))))
		return 0.0;
	return round((double)arrayLineItems.GetColumn(itemnumber, StrDbl(~PRICE)) * (double)arrayLineItems.GetColumn(itemnumber, StrDbl(~QTY)), 2);
}

void CreateInvoiceWindow::CalcInvoiceTotal()
{
	if (arrayLineItems.GetCount() < 1) 
	{
		txtNonTaxable.SetData(0.0);
		txtTaxable.SetData(0.0);
		txtSalesTax.SetData(0.0);
		txtGrandtotal.SetData(0.0);
		return;
	}
	double nonTaxable = 0.0, taxable = 0.0, salestax = 0.0, grandtotal = 0.0;
	// ERROR in SalesTax Calculation!!!!
	for (int i = 0; i < arrayLineItems.GetCount(); i++)
	{
		if (optCustTaxable.Get() == true && arrayLineItems.Get(i, ISTAXABLE)  == 1) {
			taxable += round((double)arrayLineItems.Get(i, PRICE) * (double)arrayLineItems.Get(i, QTY), 2);
		}
		else 	nonTaxable += round((double)arrayLineItems.Get(i, PRICE) * (double)arrayLineItems.Get(i, QTY), 2);
	}
	if (optCustTaxable.Get() == true) salestax = round( taxable * txtTaxRate / 100, 2);
	else salestax = 0.00;
	
	grandtotal = nonTaxable + taxable + salestax;
	txtNonTaxable.SetText(Format("$%2!nl",nonTaxable)); // .SetData
	txtTaxable.SetText(Format("$%2!nl",taxable));
	txtSalesTax.SetText(Format("$%2!nl",salestax));
	txtGrandtotal.SetText(Format("$%2!nl",grandtotal));
}

void CreateInvoiceWindow::DeleteRow()
{
	int id = arrayLineItems.GetKey();
	if(IsNull(id))
		return;

	arrayLineItems.DoRemove();
	CalcInvoiceTotal();
}
	