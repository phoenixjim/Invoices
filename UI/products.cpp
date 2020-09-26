#include "DBUI.h"
#include "../configs.h"

class AddProduct : public WithProductAddLayout<TopWindow> {
	public:
		SqlCtrls ctrls;
		typedef AddProduct CLASSNAME;
		AddProduct();
};

/*

*/

AddProduct::AddProduct()
{
	CtrlLayoutOKCancel(*this, "Add Product");
	ctrls 
		(PROD_ID, txtProductID)
		(INVOICEID, txtInvoiceNo)
		(PRODNAME, txtProductName)
		(PRODDESCRIPTION, txtDescription)
		(DATEPURCHASED, dtpDatePurchased)
		(COST, txtProductCost)
		;
		/*
	INT_ (PROD_ID)	NOT_NULL PRIMARY_KEY AUTO_INCREMENT
	STRING_ (PRODNAME, 150)	NOT_NULL
	STRING_ (PRODDESCRIPTION, 200)
	INT_ (DATEPURCHASED)
	DOUBLE_ (COST)	NOT_NULL
	INT_ (INVOICEID)	NOT_NULL
*/
}

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
	btnAddProduct << [=] { AddNewProduct(); };
	
	// btnUpdateProduct << [=] { btnUpdateProductClick(); };
	
	// btnShowAllProduct << [=] { btnShowAllProductClick(); };
	
	// btnProductRange << [=] { btnProductRangeClick(); };
		
	ProductArray.SetTable(PRODUCTS, PROD_ID);
		
	// ProductArray.Join(BOOK_ID, book); // joins id from other db to this id
	ProductArray.AddColumn(PRODNAME, "Name");
	ProductArray.AddColumn(PRODDESCRIPTION, "Description");
	ProductArray.AddColumn(DATEPURCHASED, "Date Purchased");
	ProductArray.AddColumn(COST, "Cost");
	ProductArray.AddColumn(INVOICEID, "Invoice#");
	ProductArray.ColumnWidths("100 100 40 30 30");
	ProductArray.SetOrderBy(PROD_ID).Removing();
	ProductArray.Query();

	// ProductArray.WhenBar = THISBACK(BorrowMenu);
	ProductArray.WhenLeftDouble = [=] { EditRow();};
	// ProductArray.GoEndPostQuery();
}

void ProductsWindow::AddNewProduct()
{

	PromptOK(__func__);
}
/*
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
*/
void ProductsWindow::EditRow() // CONTINUE HERE
{
	int idNum;
	if(!ProductArray.IsCursor())
		return;
	idNum = ProductArray.GetKey();
	if (IsNull(idNum))
		return;
	AddProduct dlg;
	SQL * Select(dlg.ctrls).From(PRODUCTS).Where(PROD_ID == idNum);
	if(!dlg.ctrls.Fetch(SQL))
		return;
	if(dlg.Run() != IDOK)
		return;
	SQL * dlg.ctrls.Update(PRODUCTS).Where(PROD_ID == idNum);
	ProductArray.ReQuery();
	ProductArray.FindSetCursor(idNum);
}
