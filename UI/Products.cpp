#include "DBUI.h"
#include "../Utils/configs.h"

class AddProduct : public WithProductAddLayout<TopWindow> {
	public:
		SqlCtrls ctrls;
		typedef AddProduct CLASSNAME;
		AddProduct(String title);
};


AddProduct::AddProduct(String title)
{
	CtrlLayoutOKCancel(*this, title);
	ctrls 
//		(PROD_ID, txtProductID)
		(INVOICEID, txtInvoiceNo)
		(PRODNAME, txtProductName)
		(PRODDESCRIPTION, txtDescription)
		(DATEPURCHASED, dtpDatePurchased)
		(COST, txtProductCost)
		;
	dtpDatePurchased.SetConvert(DateIntConvert());
}

ProductsWindow::ProductsWindow() {
	CtrlLayout(*this, "Products");	
	btnAddProduct << [=] { AddNewProduct(); };
	// btnUpdateProduct << [=] { btnUpdateProductClick(); };
	// btnShowAllProduct << [=] { btnShowAllProductClick(); };
	// btnProductRange << [=] { btnProductRangeClick(); };
		
	ProductArray.SetTable(PRODUCTS, PROD_ID);
		
	// ProductArray.Join(BOOK_ID, book); // joins id from other db to this id
	ProductArray.AddColumn(PRODNAME, "Name");
	ProductArray.AddColumn(PRODDESCRIPTION, "Description");
	ProductArray.AddColumn(DATEPURCHASED, "Date Purchased").SetConvert(DateIntConvert());
	ProductArray.AddColumn(COST, "Cost").SetConvert(ConvDouble());
	ProductArray.AddColumn(INVOICEID, "Invoice#");
	ProductArray.ColumnWidths("100 100 40 30 30");
	ProductArray.SetOrderBy(PROD_ID).Appending().Removing();
	ProductArray.Query();

	ProductArray.WhenLeftDouble = [=] { EditRow();};
}

void ProductsWindow::EditRow() // CONTINUE HERE
{
	int idNum;
	if(!ProductArray.IsCursor())
		return;
	idNum = ProductArray.GetKey();
	if (IsNull(idNum))
		return;
	AddProduct dlg("Edit Product");
	
	SQL * Select(dlg.ctrls).From(PRODUCTS).Where(PROD_ID == idNum);
	if(!dlg.ctrls.Fetch(SQL))
		return;
	if(dlg.Run() != IDOK)
		return;
	SQL * dlg.ctrls.Update(PRODUCTS).Where(PROD_ID == idNum);
	ProductArray.ReQuery();
	ProductArray.FindSetCursor(idNum);
}

void ProductsWindow::AddNewProduct()
{
	AddProduct dlg("Add Product");
	if(dlg.Execute() != IDOK)
		return;
	SQL * dlg.ctrls.Insert(PRODUCTS);

    int id = SQL.GetInsertedId();

    ProductArray.ReQuery();
    ProductArray.FindSetCursor(id);
}
