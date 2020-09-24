#include "UI/DBUI.h"
#include "configs.h"

// ListLineItems
LineItemsWindow::LineItemsWindow() {
	CtrlLayout(*this, "List Line Items");
	
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
	LineItemsArray.SetOrderBy(LINEITEM_ID);

	LineItemsArray.Query();
	};
