#include "DBUI.h"

CreateInvoiceWindow::CreateInvoiceWindow()
 { 
 	CtrlLayout(*this, "Create Invoice");
 	txtTerms.SetText("Due On Receipt");
 	txtTaxRate.SetData(0.08);
 	
 	btnAdd << [=] { AddItem(); };
 	btnDelete << [=] { ClearItem(); };
 	ok << [=] { SaveInvoice(); };
 	cancel << [=] { CancelInvoice(); };
 	btnSubtract << [=] { AdjustPrice(); };
 	btnCalcTotals << [=] {CalcInvoiceTotal(); };
 	
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
	/*
	SQL * Select(TAXABLE).From(CUSTOMERS).Where(CUST_ID == idNum);
	SQL.Fetch();
	optCustTaxable.Set(SQL[0]);
	*/
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

double CreateInvoiceWindow::round(double d, int n) {
	return floor(d * ipow10(n) + 0.5) / ipow10(n);
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
	int custId = cbCustomers.GetIndex();

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
	// Invoices p = GetMainWindow();
	// UpdateTables();

	ClearItem();
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
	txtNonTaxable.SetData(nonTaxable);
	txtTaxable.SetData(taxable);
	txtSalesTax.SetData(salestax);
	txtGrandtotal.SetData(grandtotal);
}

void CreateInvoiceWindow::DeleteRow()
{
	int id = arrayLineItems.GetKey();
	if(IsNull(id))
		return;

	arrayLineItems.DoRemove();
	CalcInvoiceTotal();
}
	