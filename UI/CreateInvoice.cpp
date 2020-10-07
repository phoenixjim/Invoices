#include "DBUI.h"

CreateInvoiceWindow::CreateInvoiceWindow()
 { 
 	CtrlLayout(*this, "Create Invoice");
 	txtTerms.SetText("Due On Receipt");
 	txtTaxRate.SetData(0.08);
 	printInvoice = 0;
 	
 	btnAdd << [=] { AddItem(); };
 	btnDelete << [=] { ClearItem(); };
 	ok << [=] { SaveInvoice(); };
 	cancel << [=] { CancelInvoice(); };
 	btnSubtract << [=] { AdjustPrice(); };
 	btnPrintSave << [=] { PrintInvoice(); };
 	
 	arrayLineItems.AddColumn(PRODUCTNAME, "Name", 40);
 	arrayLineItems.AddColumn(DESCRIPTION, "Description", 80);
 	arrayLineItems.AddColumn(PRICE, "Price", 20).SetConvert(ConvDouble());
 	arrayLineItems.AddColumn(QTY, "Qty", 15);
 	arrayLineItems.AddColumn(ISTAXABLE, "Tax?", 15);
 	arrayLineItems.AddColumn(TOTAL, "Total", 20).SetConvert(ConvDouble());
 	arrayLineItems.Appending() .Removing();
 
 	SQL * Select(CUST_ID, CUSTNAME).From(CUSTOMERS);
 	while (SQL.Fetch())
 	{
 		cbCustomers.Add(SQL[CUST_ID], SQL[CUSTNAME]);
 	}
	
	cbProducts.Add("Service");
	cbProducts.Add("Part");
	cbProducts.Add("Tip");
	cbProducts.Add("Refund");
	cbProducts.Add("Note");

	SQL.Execute("Select MAX(INVOICENUMBER) From INVOICES");
	SQL.Fetch();
	nextInvoice = (int)SQL[0] + 1; // stol(SQL[0].ToString().ToStd()) + 1;
	txtInvoice = nextInvoice; // .SetText(IntStr64(nextInvoice));
	cbCustomers.WhenAction << [=] { CustChanged(); };
	cbProducts.WhenAction << [=] { ProdChanged(); };
	dtpBillDate.SetConvert(DateIntConvert());
 }
 
void CreateInvoiceWindow::CustChanged()
{
	int idNum = 1;
	idNum += cbCustomers.GetIndex();
	if (IsNull(idNum) || idNum < 1)
		return;
	optCustTaxable.Set(SQL % Select(TAXABLE).From(CUSTOMERS).Where(CUST_ID == idNum));
}

void CreateInvoiceWindow::ProdChanged()
{
	int idNum = cbProducts.GetIndex() + 1;
	switch(idNum)
	{
		case Service:
		case Part:
		case Refund:
			optProdTaxable.Set(true);
			break;
			
		case Gift:
		case Note:
			optProdTaxable.Set(false);
			break;
	}
}

void CreateInvoiceWindow::AdjustPrice()
{
	if (IsNull(txtPrice)) return; // txtPrice.GetData().IsNull()) return;
	
	double newPrice = round((double)txtPrice / 1.08, 2);
	txtPrice = newPrice;
}

void CreateInvoiceWindow::SaveInvoice()
{
	int idNum = arrayLineItems.GetCount();

	if (idNum < 1 || IsNull(cbCustomers) || IsNull(dtpBillDate))
	{
		PromptOK("Are you missing something? (Customer, Date or items)");
		return;
	}
	double nonTaxable = 0.0, taxable = 0.0, salestax = 0.0, grandTotal = 0.0;

	for (int i = 0; i < idNum; i++)
	{
		if (optCustTaxable.Get() == true && arrayLineItems.Get(i, ISTAXABLE)  == 1) {
			taxable = round((double)arrayLineItems.Get(i, PRICE) * (double)arrayLineItems.Get(i, QTY), 2);
		}
		else 	nonTaxable = round((double)arrayLineItems.Get(i, PRICE) * (double)arrayLineItems.Get(i, QTY), 2);
		salestax = round(taxable * (double)txtTaxRate, 2);
		
		grandTotal += salestax + nonTaxable + taxable;
		SQL * Insert(LINEITEMS)
		(PRODUCTNAME, arrayLineItems.Get(i,PRODUCTNAME))
		(DESCRIPTION, arrayLineItems.Get(i, DESCRIPTION))
		(PRICE, (double)arrayLineItems.Get(i, PRICE))
		(QTY, (int)arrayLineItems.Get(i, QTY))
		(TOTAL, (double)arrayLineItems.Get(i, TOTAL))
		(INVOICEIDNUMBER, (int64)txtInvoice)
		(ISTAXABLE, (int)arrayLineItems.Get(i, ISTAXABLE));
	}
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
		(STATUS, 0);
	printInvoice = SQL.GetInsertedId();
	ClearItem();
}

void CreateInvoiceWindow::PrintInvoice()
{
	String invoiceQTF;
	Report myInvoice;
	
	SaveInvoice();
	
	// printInvoice = InvoicesArray.GetKey();
	if (printInvoice == 0 || IsNull(printInvoice))
	{
		Exclamation("No invoice number saved!");
		return;
	}
	
	Sql custSQL;
	Sql linesSQL;
	Sql invoiceSQL;

	String header = "[ {{5000:5000f0;g0; [s0;%% " + myConfig.companyname + " ] :: [s0;>%% [@6 "; // Add paid message right aligned if paid
	String footer = "[ $$0,0#00000000000000000000000000000000:Default] [ [s0;= [@5;0 Thank you for your business!]&][s0;= [@5;0 Company Name Company Address City, State Zip Phone Email]]]";
	
	invoiceSQL * SelectAll().From( INVOICES ).Where( INVOICE_ID == printInvoice );
	
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
	invoiceQTF << "[ [ {{729:2603:1666:1666:1666:1670@L|1 [ Item]:: [ Name]:: [> Price]:: [> Quantity]:: [> Taxable]::|1 [> Subtotal]:: [ ]::-3 [ Description]::-2 [ ]::-1 [ ]:: [ ]:: [ ]}}]]&";
	int linenumber = 0;

	while (linesSQL.Fetch())
	{
		if (linenumber % 2) invoiceQTF << "[ [ {{729:2603:1666:1666:1666:1670@L|1 [ ";
		else invoiceQTF << "[ [ {{729:2603:1666:1666:1666:1670@W|1 [ ";
		invoiceQTF << ++linenumber << "]:: [ " << linesSQL[PRODUCTNAME] << "]:: [> " << Format("$%2!nl",linesSQL[PRICE]) <<
			"]:: [> " << linesSQL[QTY] << "]:: [> "<< ( linesSQL[ISTAXABLE] ? "T" : "" ) << "]::|1 [> " << 
			Format("$%2!nl",linesSQL[TOTAL]) << "]:: [ ]::-3 [ " << linesSQL[DESCRIPTION] << "]::-2 [ ]::-1 [ ]:: [ ]:: [ ]}}]]&";
	}
	
	// Minor adjustment needed to align dollar column
	double amtPaid = (IsNull(invoiceSQL[AMTPAID]) ?  (double)0.00 : (double)invoiceSQL[AMTPAID]);
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Taxable Sub:]::a4/15 [> " << Format("$%2!nl",invoiceSQL[TAXABLESUB]) << "]}}]]&";
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ NonTaxable Sub:]::a4/15 [> " << Format("$%2!nl",invoiceSQL[NONTAXABLESUB]) << "]}}]]&";
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Tax:]::a4/15 [> " << Format("$%2!nl",invoiceSQL[TAX]) << "]}}]]&";
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Total:]::a4/15 [> " << Format("$%2!nl",invoiceSQL[GRANDTOTAL]) << "]}}]]&";
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Amount Paid:]::a4/15 [> " << Format("$%2!nl", amtPaid) << "]}}]]&";
	invoiceQTF << "[ [ {{729:2603:1666:866:2466:1695f0;g0; [ ]:: [ ]:: [ ]:: [ ]:: [ Balance Due:]::a4/15 [> " << Format("$%2!nl",(double)invoiceSQL[GRANDTOTAL] - amtPaid) << "]}}]]&";

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
	arrayLineItems.Add(cbProducts.GetData().ToString(),
		txtDescription.GetText().ToString(),
		StrDbl(txtPrice.GetText().ToString()),
		StrInt(txtQty.GetText().ToString()), 
		optProdTaxable.Get(),
		StrDbl(txtPrice.GetText().ToString()) * StrInt(txtQty.GetText().ToString()));
	CalcInvoiceTotal();
}

void CreateInvoiceWindow::ClearItem()
{
	txtDescription.SetText("");
	txtPrice.SetText("");
	txtQty.SetText("");
	cbProducts.SetIndex(0);
	optProdTaxable.Set(0);
	arrayLineItems.Clear();
	CalcInvoiceTotal();
}

double CreateInvoiceWindow::CalcItemTotal(int itemnumber)
{
	if (IsNull(arrayLineItems.GetColumn(itemnumber, StrInt(~PRICE))) || IsNull(arrayLineItems.GetColumn(itemnumber, StrInt(~QTY))))
		return 0.0;
	return round((double)arrayLineItems.GetColumn(itemnumber, StrInt(~PRICE)) * (double)arrayLineItems.GetColumn(itemnumber, StrInt(~QTY)), 2);
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
	for (int i = 0; i < arrayLineItems.GetCount(); i++)
	{
		if (optCustTaxable.Get() == true && arrayLineItems.Get(i, ISTAXABLE)  == 1) {
			taxable += round((double)arrayLineItems.Get(i, PRICE) * (double)arrayLineItems.Get(i, QTY), 2);
		}
		else 	nonTaxable += round((double)arrayLineItems.Get(i, PRICE) * (double)arrayLineItems.Get(i, QTY), 2);
		salestax += round(taxable * (double)txtTaxRate, 2);
	}
	grandtotal = nonTaxable + taxable + salestax;
	txtNonTaxable.SetText(Format("%2!nl",nonTaxable)); // .SetData
	txtTaxable.SetText(Format("%2!nl",taxable));
	txtSalesTax.SetText(Format("%2!nl",salestax));
	txtGrandtotal.SetText(Format("%2!nl",grandtotal));
}

void CreateInvoiceWindow::DeleteRow()
{
	int id = arrayLineItems.GetKey();
	if(IsNull(id))
		return;

	arrayLineItems.DoRemove();
	CalcInvoiceTotal();
}
	