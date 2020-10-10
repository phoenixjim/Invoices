#include "DBUI.h"
#include "../Utils/configs.h"
#include <plugin/sqlite3/Sqlite3.h>

class AddLineItem : public WithLineItemAddLayout<TopWindow> {
		void ProdChanged();
public:
		SqlCtrls ctrls;
		typedef AddLineItem CLASSNAME;
		AddLineItem();
};

AddLineItem::AddLineItem()
{
	CtrlLayoutOKCancel(*this, "Add Item");
	ctrls
		(INVOICEIDNUMBER, txtInvoiceNum)
		(PRODUCTNAME, cbProducts)
		(DESCRIPTION, txtDescription)
		(PRICE, txtPrice)
		(QTY, txtQty)
		(TOTAL, txtTotal)
		(ISTAXABLE, optProdTaxable)
	;
	// cbProducts.SetConvert(ConvLineItem());
	
	cbProducts.Add("Service");
	cbProducts.Add("Part");
	cbProducts.Add("Tip");
	cbProducts.Add("Refund");
	cbProducts.Add("Note");
	
	cbProducts.WhenAction << [=] { ProdChanged(); };
}

void AddLineItem::ProdChanged()
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

LineItemsWindow::LineItemsWindow() {
	CtrlLayout(*this, "List Line Items");
	btnAddItem << [=] { AddNewItem(); };
	
	LineItemsArray.SetTable(LINEITEMS, LINEITEM_ID);
		
	//LineItemsArray.Join(BOOK_ID, book); // joins id from other db to this id
	LineItemsArray.AddColumn(PRODUCTNAME, "Product Name");
	LineItemsArray.AddColumn(DESCRIPTION, "Description");
	LineItemsArray.AddColumn(PRICE, "Price").SetConvert(ConvDouble());
	LineItemsArray.AddColumn(QTY, "Qty");
	LineItemsArray.AddColumn(TOTAL, "Total").SetConvert(ConvDouble());
	LineItemsArray.AddColumn(INVOICEIDNUMBER, "Invoice#");
	LineItemsArray.AddColumn(ISTAXABLE, "Taxable?");
	//LineItemsArray.ColumnWidths("5 5 10 12 5 5 5 5 5 5");
	LineItemsArray.Appending().Removing();
	LineItemsArray.SetOrderBy(LINEITEM_ID);
	
	LineItemsArray.WhenLeftDouble = [=] { EditRow(); };

	LineItemsArray.Query();
	};

// NOTE: For the following, after ok, should update numbers in associated Invoice.
/*
void LineItemsWindow::Open(Ctrl* owner)
{
	LineItemsArray.ReQuery();
	LineItemsArray.GoBegin();
	
	TopWindow::Open(owner);
}
*/

void LineItemsWindow::EditRow()
{
	// should only edit full invoices, make editinvoicewindow
	int idNum;
	if(!LineItemsArray.IsCursor())
		return;
	idNum = LineItemsArray.GetKey();
	if (IsNull(idNum))
		return;
	/*
	Sql invoicesql;
	invoicesql * SelectAll().From(INVOICES).Where(INVOICEIDNUMBER == INVOICENUMBER);
	invoicesql.Fetch();
	
	if (invoicesql.GetRowsProcessed() < 1 || (double)invoicesql[0][AMTPAID] > 0.0) {
		Exclamation("Payment already received, no changes allowed.");
		return; // if payment received, set in stone
	} */
	AddLineItem dlg;
	dlg.Title("Edit Item");

	SQL * Select(dlg.ctrls).From(LINEITEMS).Where(LINEITEM_ID == idNum);
	if(!dlg.ctrls.Fetch(SQL))
		return;
	
	if(dlg.Run() != IDOK)
		return;
	
	SQL * dlg.ctrls.Update(LINEITEMS).Where(LINEITEM_ID == idNum);
	// Update invoice totals, querying for all lineitems for this invoice
	
	LineItemsArray.ReQuery();
	LineItemsArray.FindSetCursor(idNum);
}
void LineItemsWindow::AddNewItem()
{
    AddLineItem dlg;
    dlg.Title("New Item");
    if(dlg.Execute() != IDOK)
        return;
    SQL * dlg.ctrls.Insert(LINEITEMS);
	
	int id = SQL.GetInsertedId();

    LineItemsArray.ReQuery();
    LineItemsArray.FindSetCursor(id);
}

