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
 	
 	arrayLineItems.AddColumn("Name", 40);
 	arrayLineItems.AddColumn("Description", 80);
 	arrayLineItems.AddColumn("Price", 20).SetConvert(ConvDouble());
 	arrayLineItems.AddColumn("Qty", 15);
 	arrayLineItems.AddColumn("Tax?", 15);
 	arrayLineItems.AddColumn("Total", 20);
 	arrayLineItems.Removing();
 
 	SQL * Select(CUST_ID, CUSTNAME).From(CUSTOMERS);
 	while (SQL.Fetch())
 	{
 		cbCustomers.Add(~SQL[CUST_ID], ~SQL[CUSTNAME]);
 	}
	
	cbProducts.Add("Service");
	cbProducts.Add("Part");
	cbProducts.Add("Tip");
	cbProducts.Add("Refund");
	cbProducts.Add("Note");

	SQL.Execute("Select MAX(INVOICENUMBER) From INVOICES");
	SQL.Fetch();
	nextInvoice = stol(SQL[0].ToString().ToStd()) + 1;
	txtInvoice.SetText(IntStr64(nextInvoice));
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

	SQL * Select(TAXABLE).From(CUSTOMERS).Where(CUST_ID == idNum);
	SQL.Fetch();
	optCustTaxable.Set(SQL[0]);
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
	if (txtPrice.GetData().IsNull()) return;
	
	double newPrice = round(StrDbl(txtPrice.GetData().ToString()) / 1.08, 2);
	txtPrice.SetData(newPrice);
	
}

void CreateInvoiceWindow::SaveInvoice()
{
	int idNum = arrayLineItems.GetCount();

	if (idNum < 1 || cbCustomers.GetData().IsNull() || dtpBillDate.GetData().IsNull())
	{
		PromptOK("Are you missing something? (Customer, Date or items)");
		return;
	}
	double nonTaxable = 0.0, taxable = 0.0, salestax = 0.0, grandTotal = 0.0;

	for (int i = 0; i < idNum; i++)
	{
		if (optCustTaxable.Get() == true && arrayLineItems.GetColumn(i, 4)  == 1) {
			taxable = round(StrDbl(arrayLineItems.GetColumn(i, 2).ToString()) * StrDbl(arrayLineItems.GetColumn(i, 3).ToString()), 2);
		}
		else 	nonTaxable = round(StrDbl(arrayLineItems.GetColumn(i, 2).ToString()) * StrDbl(arrayLineItems.GetColumn(i, 3).ToString()), 2);
		salestax = round(taxable * StrDbl(txtTaxRate.GetData().ToString()), 2);
		
		grandTotal += salestax + nonTaxable + taxable;
		SQL * Insert(LINEITEMS)
		(PRODUCTNAME, arrayLineItems.GetColumn(i,0).ToString())
		(DESCRIPTION, arrayLineItems.GetColumn(i,1).ToString())
		(PRICE, StrDbl(arrayLineItems.GetColumn(i,2).ToString()))
		(QTY, StrInt(arrayLineItems.GetColumn(i,3).ToString()))
		(TOTAL, StrDbl(arrayLineItems.GetColumn(i,5).ToString()))
		(INVOICEIDNUMBER, StrInt64(txtInvoice.GetData().ToString()))
		(ISTAXABLE, StrInt(arrayLineItems.GetColumn(i,4).ToString()));
	}
	int custId = cbCustomers.GetIndex();

	SQL * Insert(INVOICES)
		(INVOICENUMBER, StrInt64(txtInvoice.GetData().ToString()))
		(CUSTOMERID, custId)
		(TRANSACTIONDATE, dtpBillDate.GetData())
		(TERMS, txtTerms.GetData().ToString())
		(NONTAXABLESUB, nonTaxable)
		(TAXABLESUB, taxable)
		(TAX, salestax)
		(GRANDTOTAL, grandTotal)
		(AMTPAID, 0.0)
		(STATUS, 0);
	Invoices p = GetMainWindow();
	//  UpdateTables();

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
	if (arrayLineItems.GetColumn(itemnumber, 2).IsNull() || arrayLineItems.GetColumn(itemnumber, 3).IsNull())
		return 0.0;
	double retval = StrDbl(arrayLineItems.GetColumn(itemnumber, 2).ToString()) * StrDbl(arrayLineItems.GetColumn(itemnumber, 3).ToString());
	return retval;
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
		if (optCustTaxable.Get() == true && arrayLineItems.GetColumn(i, 4)  == 1) {
			taxable += round(StrDbl(arrayLineItems.GetColumn(i, 2).ToString()) * StrDbl(arrayLineItems.GetColumn(i, 3).ToString()), 2);
		}
		else 	nonTaxable += round(StrDbl(arrayLineItems.GetColumn(i, 2).ToString()) * StrDbl(arrayLineItems.GetColumn(i, 3).ToString()), 2);
		salestax += round(taxable * StrDbl(txtTaxRate.GetData().ToString()), 2);
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
	