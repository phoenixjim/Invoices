#include "customers.h"
#define MODEL <Invoices/Tables/Invoices.sch>
#include <Sql/sch_source.h>

#undef MODEL

CustomersWindow::CustomersWindow() {


	CtrlLayout(*this, "Customers");
	
	btnAddCustomer << [=] { btnAddCustomerClick(); }; // THISBACK is not needed in c++11 world and could be replaced with lambda.
	btnSearchCustomer << [=] { btnSearchCustomerClick(); }; // assisted by forum user Klugier
	btnUpdateCustomer << [=] { btnUpdateCustomerClick(); };
	
	// btnFake << [=] { FakeStub(); };
	
	if(FileExists(myConfig.configfile))
	{
        VectorMap<String, String> cfg = LoadIniFile(myConfig.configfile);
        myConfig.DBFile = cfg.Get("DBFile", Null);
	}
	else {
		myConfig.DBFile = myConfig.SelectDB();
	}

	Sqlite3Session sqlite3;
	if(!sqlite3.Open(myConfig.DBFile)) {
		Exclamation("Can't create or open database file\n");
		return;
	}
	SQL = sqlite3;
	Sql sql;
	CustArray.SetTable(CUSTOMERS, CUST_ID);
		
	// CustArray.Join(BOOK_ID, book); // joins id from other db to this id
	CustArray.AddColumn(CUSTNAME, "Name"); // .SetConvert(DateIntConvert());
	CustArray.AddColumn(EMAIL, "Email"); // .SetConvert(DateIntConvert());
	CustArray.AddColumn(CONTACT, "Phone");
	CustArray.AddColumn(ADDRESS, "Address");
	CustArray.AddColumn(CITY, "City");
	CustArray.AddColumn(STATE, "State");
	CustArray.AddColumn(ZIP, "Zip");
	CustArray.AddColumn(TAXABLE, "Taxable?");
	CustArray.ColumnWidths("40 40 20 50 20 15 10 5");
	CustArray.SetOrderBy(CUST_ID);
	
	// sql.Execute("select * from CUSTOMERS");
	// sql * SelectAll().From(CUSTOMERS);
	
	// while(sql.Fetch()) 	CustArray.Add(sql);
	CustArray.Query();
	
	// CustArray.WhenBar
	CustArray.WhenLeftDouble = [=] { EditRow(); };
	
	// CustArray.GoEndPostQuery();

}

void CustomersWindow::btnAddCustomerClick()
{

	PromptOK(__func__);
}

void CustomersWindow::btnUpdateCustomerClick()
{
	PromptOK(__func__);
}

void CustomersWindow::btnSearchCustomerClick()
{
	PromptOK(__func__);
}

void CustomersWindow::EditRow()
{
	/*
	ITEM(EditString, txtCustSearch, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(404, 19))
	ITEM(EditString, txtCustName, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(76, 19))
	ITEM(EditString, txtCustEmail, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(116, 19))
	ITEM(EditString, txtCustPhone, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(156, 19))
	ITEM(EditString, txtCustAddress, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(196, 19))
	ITEM(EditString, txtCustCity, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(232, 19))
	ITEM(EditString, txtCustState, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(272, 19))
	ITEM(EditString, txtCustZip, MaxChars(10).SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 64).TopPosZ(308, 19))
	*/
}
