#include "DBUI.h"
#include <string>

CreateInvoiceWindow::CreateInvoiceWindow()
 { 
 	CtrlLayout(*this, "Create Invoice");
 	txtTerms.SetText("Due On Receipt");
 	
 	btnAdd << [=] { AddItem(); };
 	btnDelete << [=] { ClearItem(); };
 	ok << [=] { SaveInvoice(); };
 	cancel << [=] { CancelInvoice(); };
 	btnSubtract << [=] { AdjustPrice(); };
 	
 	arrayLineItems.AddColumn("Name", 40);
 	arrayLineItems.AddColumn("Description", 80);
 	arrayLineItems.AddColumn("Price", 20);
 	arrayLineItems.AddColumn("Qty", 15);
 	arrayLineItems.AddColumn("Tax?", 15);
 
 	SQL * Select(CUST_ID, CUSTNAME).From(CUSTOMERS);
 	while (SQL.Fetch())
 	{
 		cbCustomers.Add(~SQL[CUST_ID], ~SQL[CUSTNAME]);
 	}
 	
	cbProducts.Add(Service, "Service");
	cbProducts.Add(Part, "Part");
	cbProducts.Add(Gift, "Tip");
	cbProducts.Add(Refund, "Refund");

	SQL.Execute("Select MAX(INVOICENUMBER) From INVOICES");
	SQL.Fetch();
	nextInvoice = stol(SQL[0].ToString().ToStd()) + 1;
	txtInvoice.SetText(std::to_string(nextInvoice));
	cbCustomers.WhenAction << [=] { CustChanged(); };
	cbProducts.WhenAction << [=] { ProdChanged(); };
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
			optProdTaxable.Set(false);
			break;
	}
}

void CreateInvoiceWindow::AdjustPrice()
{
}

void CreateInvoiceWindow::SaveInvoice()
{
}

void CreateInvoiceWindow::CancelInvoice()
{
}

void CreateInvoiceWindow::AddItem()
{
}

void CreateInvoiceWindow::ClearItem()
{
}