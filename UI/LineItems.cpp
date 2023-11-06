#include "DBUI.h"
#include "../Utils/configs.h"
#include <plugin/sqlite3/Sqlite3.h>

class AddLineItem : public WithLineItemAddLayout<TopWindow> {
	Configs myConfig;
	void ProdChanged();
	void QtyChanged();
	void PriceChanged();
	void AdjustPrice();
public:
	SqlCtrls ctrls;
	AddLineItem();
	virtual void Paint(Draw& w) {
		w.DrawRect(GetSize(), COLOR); // <= enter your background color here
	}
};

AddLineItem::AddLineItem()
{
	CtrlLayoutOKCancel(*this, "Add Item");
	ctrls
		(LINEITEM_ID, txtLineItemId)
		(INVOICEIDNUMBER, txtInvoiceNum)
		(PRODUCTNAME, cbProducts)
		(DESCRIPTION, txtDescription)
		(PRICE, txtPrice)
		(QTY, txtQty)
		(TOTAL, txtTotal)
		(ISTAXABLE, optProdTaxable)
	;

	SQL * Select(TYPENUM, TYPENAME).From(TYPES);
	
	while (SQL.Fetch())
	{
		cbProducts.Add(SQL[TYPENUM], SQL[TYPENAME]);
	}
	
	Sql tempSql;
	tempSql.Execute("Select MAX(INVOICENUMBER) From INVOICES");
	tempSql.Fetch();
	int maxInv = (int)tempSql[0]; // stol(SQL[0].ToString().ToStd()) + 1;
	tempSql.Execute("Select MIN(INVOICENUMBER) From INVOICES");
	tempSql.Fetch();
	int minInv = (int)tempSql[0]; // stol(SQL[0].ToString().ToStd()) + 1;
	txtInvoiceNum.Max(maxInv).Min(minInv).SetData(maxInv);
	cbProducts.WhenAction << [=] { ProdChanged(); };
	txtPrice.WhenAction << [=] { PriceChanged(); };
	txtQty.WhenAction << [=] { QtyChanged(); };
	txtTotal.SetConvert(ConvCurrency());
	btnTaxReduce << [=] { AdjustPrice(); };
	lDesc.SetInk(TXTCOLOR);
	lPrice.SetInk(TXTCOLOR);
	lQty.SetInk(TXTCOLOR);
	lTotal.SetInk(TXTCOLOR);
	lName.SetInk(TXTCOLOR);
	lNumber.SetInk(TXTCOLOR);
	optProdTaxable.SetColor(TXTCOLOR);
}

void AddLineItem::QtyChanged()
{
	if (IsNull(txtPrice) || IsNull(txtQty)) return;
	txtTotal = round((double)txtPrice * (double)txtQty, 2);
}

void AddLineItem::PriceChanged()
{
	if (IsNull(txtPrice) || IsNull(txtQty)) return;
	txtTotal = round((double)txtPrice * (double)txtQty, 2);
}

void AddLineItem::ProdChanged()
{
	int idNum = cbProducts.GetIndex() + 1;
	// max taxable id is 3 currently
	if (idNum < 4) optProdTaxable.Set(true);
	else optProdTaxable.Set(false);
}

void AddLineItem::AdjustPrice() // subtract sales tax
{
	if (IsNull(txtPrice) || IsNull(txtInvoiceNum)) return;
	
	int idNum = 1;
	idNum += getCustFromInvoice(txtInvoiceNum);
	if (IsNull(idNum) || idNum < 1)
		return;
	double salestax = getTaxRate( idNum ) / 100.00;
	double newPrice = round((double)txtPrice / (double)( 1.00 + salestax ), 2);
	if ((newPrice + salestax ) < txtPrice) newPrice += 0.01;
	txtPrice.SetData( newPrice );
}

LineItemsWindow::LineItemsWindow() {
	CtrlLayout(*this, "List Line Items");
	btnAddItem << [=] { AddNewItem(); };
	btnDelItem << [=] { DeleteItem(); };
	DropList mytypes;
	cbInvoice.WhenAction << [=] { btnInvoice(); };
	LineItemsArray.SetTable(LINEITEMS, LINEITEM_ID);
		
	LineItemsArray.AddColumn(PRODUCTNAME, "Product Name").SetConvert(Single<Lookup(TYPES,TYPENUM,TYPENAME)>()).Edit(mytypes);
	LineItemsArray.AddColumn(DESCRIPTION, "Description");
	LineItemsArray.AddColumn(PRICE, "Price").SetConvert(ConvDouble()).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	LineItemsArray.AddColumn(QTY, "Qty").SetConvert(ConvDbl());
	LineItemsArray.AddColumn(TOTAL, "Total").SetConvert(ConvDouble()).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	LineItemsArray.AddColumn(INVOICEIDNUMBER, "Invoice#");
	LineItemsArray.AddColumn(ISTAXABLE, "Taxable?");
	LineItemsArray.ColumnWidths("50 175 25 15 25 20 20");
	// LineItemsArray.Appending().Removing();
	LineItemsArray.SetOrderBy(LINEITEM_ID).Appending().Removing();
	
	LineItemsArray.WhenLeftDouble = [=] { EditRow(); };
	LineItemsArray.WhenEnterKey = [=] { EditRow(); };

	Sql tempSql;
	tempSql.Execute("Select MAX(INVOICEIDNUMBER) From LINEITEMS");
	tempSql.Fetch();
	int maxInv = (int)tempSql[0]; // stol(SQL[0].ToString().ToStd()) + 1;
	tempSql.Execute("Select MIN(INVOICEIDNUMBER) From LINEITEMS Where INVOICEIDNUMBER > 0");
	tempSql.Fetch();
	int minInv = (int)tempSql[0]; // stol(SQL[0].ToString().ToStd()) + 1;

	int i = 0, invNum = minInv;

	cbInvoice.Add(i, "All");
	while (invNum <= maxInv)
 	{
 		cbInvoice.Add( ++i, IntStr(invNum++) );
 	}
 	cbInvoice.SetData(0);
	lblInvoice.SetInk(TXTCOLOR);

	LineItemsArray.Query();
	};


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
	
	dlg.txtInvoiceNum.WantFocus(false);
    dlg.txtPrice.SetData(0.0);
    dlg.txtQty.SetData(0.0);
    dlg.txtTotal.SetData(0.0);
	if(!dlg.ctrls.Fetch(SQL))
		return;
	

	SQL * Select(TYPENUM, TYPENAME).From(TYPES);
	
	while (SQL.Fetch())
	{
		dlg.cbProducts.Add(SQL[TYPENUM], SQL[TYPENAME]);
	}

	Sql invSql;
	invSql * Select(STATUS, INVOICENUMBER).From(INVOICES).Where(INVOICENUMBER == (int64)dlg.txtInvoiceNum.GetData());
	if (invSql[STATUS] == 0) return;
	
	if(dlg.Run() != IDOK)
		return;
	
	SQL * dlg.ctrls.Update(LINEITEMS).Where(LINEITEM_ID == idNum);
	// Update invoice totals, querying for all lineitems for this invoice
	
	LineItemsArray.ReQuery();
	CalcInvoiceTotal((int64)invSql[INVOICENUMBER]);
	
	LineItemsArray.FindSetCursor(idNum);
}
void LineItemsWindow::AddNewItem()
{
    AddLineItem dlg;
    dlg.Title("New Item");
    dlg.txtInvoiceNum.WantFocus(true).Enable(true);
    Sql tempSql, lineSql;

	int maxLI = 0;
	// lineSql.Execute("Select MAX(LINEITEM_ID) from LINEITEMS)");
	lineSql * Select(LINEITEM_ID).From(LINEITEMS).OrderBy(Descending( LINEITEM_ID ) ); // MAX( LINEITEM_ID )
	lineSql.Fetch();
	
	if (IsNull(lineSql[0])) return;
	maxLI = (int)lineSql[0];
	dlg.txtLineItemId.SetData( maxLI + 1 );

	tempSql.Execute("Select MAX(INVOICENUMBER) From INVOICES Where STATUS != 0");
	tempSql.Fetch();
	if (IsNull(tempSql[0])) return;
	int maxInv = (int)tempSql[0];

	tempSql.Execute("Select MIN(INVOICENUMBER) From INVOICES Where STATUS != 0");
	tempSql.Fetch();
	int minInv = (int)tempSql[0];
    dlg.txtInvoiceNum.Max(maxInv).Min(minInv).SetData(maxInv);
    dlg.txtPrice.SetData(0.0);
    dlg.txtQty.SetData(0.0);
    dlg.txtTotal.SetData(0.0);
    dlg.txtLineItemId.SetData(maxLI + 1);
    
    if(dlg.Run() != IDOK)
        return;
	
	long invoice = (int64)dlg.txtInvoiceNum.GetData();
	tempSql * Select(STATUS).From(INVOICES).Where(INVOICENUMBER == (int64)invoice);
	if (tempSql[STATUS] == 0) {
		Exclamation("Editing Voided Invoice is Not allowed.");
		return;
	}
	
    SQL * dlg.ctrls.Insert(LINEITEMS);
    /* SQL * Insert(LINEITEMS)
    	// (LINEITEM_ID, maxLI + 1)
    	(PRODUCTNAME, (int)dlg.cbProducts.Get() + 1 )
		(DESCRIPTION, dlg.txtDescription.GetData().ToString())
		(PRICE, (double)dlg.txtPrice.GetData())
		(QTY, (double)dlg.txtQty.GetData())
		(TOTAL, (double)dlg.txtTotal)
		(INVOICEIDNUMBER, (int64)dlg.txtInvoiceNum)
		(ISTAXABLE, (int)dlg.optProdTaxable);
	*/
		
	int id = SQL.GetInsertedId();
	CalcInvoiceTotal(invoice);
	LineItemsArray.ReQuery();
	LineItemsArray.FindSetCursor(id);
}

void LineItemsWindow::DeleteItem()
{
	int idNum;
	if(!LineItemsArray.IsCursor())
		return;
	idNum = LineItemsArray.GetKey();
	if (IsNull(idNum))
		return;
	if (PromptOKCancel("Are you sure? This can't be undone...")) {
		Sql li;
		li * SelectAll().From(LINEITEMS).Where(LINEITEM_ID == idNum);
		li.Fetch();
		int64 thisInvoice = li[INVOICEIDNUMBER];

		Sql inv;
		inv * SelectAll().From(INVOICES).Where(INVOICENUMBER == thisInvoice);
		inv.Fetch();
		
		li * Delete(LINEITEMS).Where(LINEITEM_ID == idNum);
		
		Sql liTmp;
		liTmp * SelectAll().From(LINEITEMS).Where(INVOICEIDNUMBER == thisInvoice);
		liTmp.Fetch();

		/*
		if (liTmp.GetRowsProcessed() < 2)
			inv * SqlUpdate(INVOICES)(STATUS, 0).Where(INVOICENUMBER == thisInvoice); // Void = 0
		else
		*/
			CalcInvoiceTotal( thisInvoice );
	}
	LineItemsArray.ReQuery();
}

void LineItemsWindow::btnInvoice()
{
	int idNum = cbInvoice.GetIndex();

	if (IsNull(idNum) || idNum < 0)
		return;
	if (idNum == 0)
		LineItemsArray.ReQuery(INVOICEIDNUMBER > 0);
	else
		LineItemsArray.ReQuery( INVOICEIDNUMBER == cbInvoice.GetValue() );
}

void LineItemsWindow::CalcInvoiceTotal(long invoice)
{
	Sql tempInv;
	tempInv * SelectAll().From(INVOICES).Where(INVOICENUMBER == (int64)invoice);
	
	Sql tempCust;
	tempCust * Select(TAXABLE).From(CUSTOMERS).Where(CUST_ID == StrInt(~tempInv[CUSTOMERID]));
	
	double nonTaxable = 0.0, taxable = 0.0, salestax = 0.0, grandtotal = 0.0;
	Sql tempLI;
	tempLI * SelectAll().From(LINEITEMS).Where((int64)invoice == INVOICEIDNUMBER);
	while (tempLI.Fetch())// for (int i = 0; i < LineItemsArray.GetCount(); i++)
	{
		if (tempCust[TAXABLE] == 1 && tempLI[ISTAXABLE] == 1) {
			taxable += round((double)tempLI[PRICE] * (double)tempLI[QTY], 2);
		}
		else 	nonTaxable += round((double)tempLI[PRICE] * (double)tempLI[QTY], 2);
	}
	if (tempCust[TAXABLE] == 1) salestax = round( taxable * (getTaxRate(tempCust[CUST_ID] ) / 100.00 ), 2);
	grandtotal = nonTaxable + taxable + salestax;
	// String exestring = "Update INVOICES SET TAXABLESUB = " + DblStr(taxable) + "NONTAXABLESUB = " + DblStr( nonTaxable) + "TAX = " + DblStr(salestax) + "GRANDTOTAL = " + DblStr( grandtotal) + "Where INVOICENUMBER == " + IntStr64(invoice);
	SQL * SqlUpdate(INVOICES)(TAXABLESUB, taxable)(NONTAXABLESUB, nonTaxable)(TAX, salestax)(GRANDTOTAL, grandtotal).Where(INVOICENUMBER == (int64)invoice);
}
