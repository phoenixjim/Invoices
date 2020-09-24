#include "UI/DBUI.h"
#include "configs.h"

ProductsWindow::ProductsWindow() {
	CtrlLayout(*this, "Products");	
/*
	ITEM(EditIntSpin, txtInvoiceNo, Min(-1).NotNull(true).SetFrame(FieldFrame()).LeftPosZ(268, 80).TopPosZ(32, 19))
	ITEM(EditString, txtProductName, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(104, 248).TopPosZ(76, 19))
	ITEM(EditString, txtProductCost, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(108, 244).TopPosZ(312, 19))
	ITEM(EditInt, txtProductID, Min(1).NotNull(true).SetEditable(false).SetFrame(FieldFrame()).LeftPosZ(104, 64).TopPosZ(32, 19))
	ITEM(DropDate, dtpDatePurchased, SetFrame(FieldFrame()).LeftPosZ(152, 104).VSizePosZ(120, 325))
	ITEM(DropDate, dtpDateStart, SetFrame(FieldFrame()).LeftPosZ(248, 104).TopPosZ(384, 19))
	ITEM(DropDate, dtpDateEnd, SetFrame(FieldFrame()).LeftPosZ(248, 104).TopPosZ(412, 19))
	ITEM(DocEdit, txtDesctription, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(108, 244).TopPosZ(168, 128))

*/
	ctrls 
		(INVOICEID, txtInvoiceNo)
		(PRODNAME, txtProductName)
		(PRODDESCRIPTION, txtDescription)
		(DATEPURCHASED, dtpDatePurchased)
		(COST, txtProductCost)
		;
	btnAddProduct << [=] { btnAddProductClick(); };
	
	btnUpdateProduct << [=] { btnUpdateProductClick(); };
	
	btnShowAllProduct << [=] { btnShowAllProductClick(); };
	
	btnProductRange << [=] { btnProductRangeClick(); };
		
	ProductArray.SetTable(PRODUCTS, PROD_ID);
		
	// ProductArray.Join(BOOK_ID, book); // joins id from other db to this id
	ProductArray.AddColumn(PRODNAME, "Name");
	ProductArray.AddColumn(PRODDESCRIPTION, "Description");
	ProductArray.AddColumn(DATEPURCHASED, "Date Purchased").SetConvert(DateIntConvert());
	ProductArray.AddColumn(COST, "Cost");
	ProductArray.AddColumn(INVOICEID, "Invoice#");
	ProductArray.ColumnWidths("100 100 40 30 30");
	ProductArray.SetOrderBy(PROD_ID);
	ProductArray.Query();

	// ProductArray.WhenBar = THISBACK(BorrowMenu);
	ProductArray.WhenLeftDouble = [=] { EditRow();};
	// ProductArray.GoEndPostQuery();
}

void ProductsWindow::btnAddProductClick()
{

	PromptOK(__func__);
}

void ProductsWindow::btnUpdateProductClick()
{
	PromptOK(__func__);
}

void ProductsWindow::btnShowAllProductClick()
{
	PromptOK(__func__);
}

void ProductsWindow::btnProductRangeClick()
{
	PromptOK(__func__);
}

void ProductsWindow::EditRow() // CONTINUE HERE
{
	int idNum;
	if(!ProductArray.IsCursor())
		return;
	idNum = ProductArray.GetKey();
	if (IsNull(idNum))
		return;
	SQL * Select(ctrls).From(CUSTOMERS).Where(CUST_ID == idNum);
	if(!ctrls.Fetch(SQL))
		return;
	/*
	if(dlg.Run() != IDOK)
		return;
	SQL * dlg.ctrls.Update(CUSTOMERS).Where(CUST_ID == idNum);
	CustArray.ReQuery();
	CustArray.FindSetCursor(idNum);
	*/
}
