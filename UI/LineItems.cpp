#include "DBUI.h"
#include "../configs.h"
#include <plugin/sqlite3/Sqlite3.h>

struct ConvLineItemCls : Convert
{
	virtual Value Scan ( const Value &q ) const;
};

Value ConvLineItemCls::Scan ( const Value &q ) const
	{
	int idNum = q;
	idNum ++;
	switch(idNum)
	{
		case 1:
			return "Service";
			break;
		case 2:
			return "Part";
			break;
		case 3:
			return "Refund";
			break;
		case 4:
			return "Tip";
			break;
		case 5:
			return "Note";
			break;
		default:
			return "Service";
	}
}

Convert& ConvLineItem()
{
	return Single<ConvLineItemCls>();
}

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
	cbProducts.Add(1, "Service");
	cbProducts.Add(2, "Part");
	cbProducts.Add(3, "Tip");
	cbProducts.Add(4, "Refund");
	cbProducts.Add(5, "Note");
	
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

void LineItemsWindow::EditRow()
{
	int idNum;
	if(!LineItemsArray.IsCursor())
		return;
	idNum = LineItemsArray.GetKey();
	if (IsNull(idNum))
		return;
	AddLineItem dlg;
	dlg.Title("Edit Item");

	SQL * Select(dlg.ctrls).From(LINEITEMS).Where(LINEITEM_ID == idNum);
	if(!dlg.ctrls.Fetch(SQL))
		return;
	int prodid = 0;
	if (SQL[PRODUCTNAME].ToString().Compare("Service") == 0)
		prodid = 0;
	else if (SQL[PRODUCTNAME].ToString().Compare("Part") == 0)
		prodid = 1;
	else if (SQL[PRODUCTNAME].ToString().Compare("Tip") == 0)
		prodid = 2;
	else if (SQL[PRODUCTNAME].ToString().Compare("Refund") == 0)// refund, or Note now
		prodid = 3;
	else prodid = 4; // Note
	dlg.cbProducts.SetIndex(prodid);
	if(dlg.Run() != IDOK)
		return;
	String ProductName;
	switch (dlg.cbProducts.GetIndex()) {
        case 0:
            ProductName = "Service";
            break;
        case 1:
            ProductName = "Part";
            break;
        case 2:
            ProductName = "Tip";
            break;
        case 3:
            ProductName = "Refund";
            break;
        case 4:
            ProductName = "Note";
            break;
    }

	SQL & ::Update( LINEITEMS ) (INVOICEIDNUMBER , StrInt(dlg.txtInvoiceNum.GetData().ToString()))
	( PRODUCTNAME , ProductName)
	( DESCRIPTION , dlg.txtDescription.GetData().ToString())
	( PRICE , StrDbl(dlg.txtPrice.GetData().ToString()))
	( QTY , StrDbl(dlg.txtQty.GetData().ToString()))
	( TOTAL , StrDbl(dlg.txtTotal.GetData().ToString()))
	( ISTAXABLE , StrInt(dlg.optProdTaxable.GetData().ToString()))
	.Where( LINEITEM_ID == idNum);

	LineItemsArray.ReQuery();
	LineItemsArray.FindSetCursor(idNum);
}
void LineItemsWindow::AddNewItem()
{
    AddLineItem dlg;
    dlg.Title("New Item");
    if(dlg.Execute() != IDOK)
        return;
    String ProductName;
    switch (dlg.cbProducts.GetIndex()) {
        case 0:
            ProductName = "Service";
            break;
        case 1:
            ProductName = "Part";
            break;
        case 2:
            ProductName = "Tip";
            break;
        case 3:
            ProductName = "Refund";
            break;
        case 4:
            ProductName = "Note";
            break;
    }
    
	SQL & ::Insert(LINEITEMS)
	(INVOICEIDNUMBER, StrInt(dlg.txtInvoiceNum.GetData().ToString()))
	(PRODUCTNAME, ProductName)
	(DESCRIPTION, dlg.txtDescription.GetData().ToString())
	(PRICE, StrDbl(dlg.txtPrice.GetData().ToString()))
	(QTY, StrDbl(dlg.txtQty.GetData().ToString()))
	(TOTAL, StrDbl(dlg.txtTotal.GetData().ToString()))
	(ISTAXABLE, StrInt(dlg.optProdTaxable.GetData().ToString()));
	
	int id = SQL.GetInsertedId();

    LineItemsArray.ReQuery();
    LineItemsArray.FindSetCursor(id);
}

