#include "DBUI.h"
#include "../Utils/configs.h"

class AddProduct : public WithProductAddLayout<TopWindow> {
	public:
		SqlCtrls ctrls;
		AddProduct(String title);
		virtual void Paint(Draw& w) {
			w.DrawRect(GetSize(), COLOR); // <= enter your background color here
    }
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
	lName.SetInk(TXTCOLOR);
	lDate.SetInk(TXTCOLOR);
	lDes.SetInk(TXTCOLOR);
	lCost.SetInk(TXTCOLOR);
	lInvNo.SetInk(TXTCOLOR);
}

ProductsWindow::ProductsWindow() {
	CtrlLayout(*this, "Products");	
	btnAddProduct << [=] { AddNewProduct(); };
	btnExportCSV << [=] { exportCSV(); };
	// btnShowAllProduct << [=] { btnShowAllProductClick(); };
	// btnProductRange << [=] { btnProductRangeClick(); };
		
	ProductArray.SetTable(PRODUCTS, PROD_ID);
		
	// ProductArray.Join(BOOK_ID, book); // joins id from other db to this id
	ProductArray.AddColumn(PROD_ID, "ID");
	ProductArray.AddColumn(PRODNAME, "Name");
	ProductArray.AddColumn(PRODDESCRIPTION, "Description");
	ProductArray.AddColumn(DATEPURCHASED, "Date Purchased").SetConvert(DateIntConvert());
	ProductArray.AddColumn(COST, "Cost").SetConvert(ConvDouble()).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	ProductArray.AddColumn(INVOICEID, "Invoice#").SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	ProductArray.ColumnWidths("5 80 120 40 30 30");
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
	if(dlg.Run() != IDOK)
		return;
	SQL * dlg.ctrls.Insert(PRODUCTS);

    int id = SQL.GetInsertedId();

    ProductArray.ReQuery();
    ProductArray.FindSetCursor(id);
}

void ProductsWindow::exportCSV() 
{
	String filename = "";

	FileSel fs;
	fs.Type("csv", "*.csv");

	#ifdef PLATFORM_WIN32
	fs.PreSelect(GetCurrentDirectory());
	#else
	fs.PreSelect(GetExeFilePath());
	#endif
	
	fs.DefaultName("PRODUCTS");
	fs.DefaultExt("csv");
	if(!fs.ExecuteSaveAs()) {
		PromptOK("No file selected.");
		return;
	}
	filename = fs;
	FileOut out(filename);
	if(!out) {
		Exclamation(t_("Failed opening ") + filename);
		return;
	}
	int i = 0;
	out << "ID,NAME,DESCRIPTION,DATE,PRICE,INVOICE\n";
	String transdate = "", cost = "";
	Sql sql;

	sql * SelectAll().From(PRODUCTS);

	while ( sql.Fetch() )
	{
		transdate = ::Format(Date( 1970, 1, 1) + (int)sql[3]); // TRANSACTIONDATE
		cost = (!sql[4].IsNull()) ? prnMoney(sql[4]) : prnMoney(0.00);

		String r = IntStr(sql[0]) << "," << sql[1] << "," << sql[2] << "," <<
			transdate << "," << cost << ","  << IntStr(sql[5]) << "\n";

		out << r;
	}
	out.Close();
}

