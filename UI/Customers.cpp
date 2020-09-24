#include "DBUI.h"
#define MODEL <Invoices/Tables/Invoices.sch>
#include <Sql/sch_source.h>

#undef MODEL
class AddCustomer : public WithCustomerAddLayout<TopWindow> {
	public:
		SqlCtrls ctrls;
		typedef AddCustomer CLASSNAME;
		AddCustomer();
};

/*
	ITEM(Option, chkTaxable, SetLabel(t_("Taxable?")).SetFont(SansSerifZ(16)).LeftPosZ(228, 108).TopPosZ(36, 16))
	ITEM(EditString, txtCustSearch, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(404, 19))
	ITEM(EditString, txtCustName, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(76, 19))
	ITEM(EditString, txtCustEmail, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(116, 19))
	ITEM(EditString, txtCustPhone, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(156, 19))
	ITEM(EditString, txtCustAddress, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(196, 19))
	ITEM(EditString, txtCustCity, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(232, 19))
	ITEM(EditString, txtCustState, SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 256).TopPosZ(272, 19))
	ITEM(EditString, txtCustZip, MaxChars(10).SetFont(SansSerifZ(16)).SetFrame(FieldFrame()).LeftPosZ(136, 64).TopPosZ(308, 19))

	STRING_ (CUSTNAME, 150) NOT_NULL
	STRING_ (EMAIL, 150)
	STRING_ (CONTACT, 150)
	STRING_ (ADDRESS, 150)
	STRING_ (CITY, 50)
	STRING_ (STATE, 20)
	STRING_ (ZIP, 10)
	INT_ (TAXABLE) NOT_NULL
*/

AddCustomer::AddCustomer()
{
	CtrlLayoutOKCancel(*this, "Add Customer");
	ctrls
		(CUSTNAME, txtCustName)
		(EMAIL, txtCustEmail)
		(CONTACT, txtCustPhone)
		(ADDRESS, txtCustAddress)
		(CITY, txtCustCity)
		(STATE, txtCustState)
		(ZIP, txtCustZip)
		(TAXABLE, chkTaxable)
	;
}


CustomersWindow::CustomersWindow() {
	CtrlLayout(*this, "Customers");
	// Major assistance on the following code from 
	btnNewCustomer << [=] { AddNewCustomer(); }; // THISBACK is not needed in c++11 world and could be replaced with lambda.
	/*
	btnSearchCustomer << [=] { btnSearchCustomerClick(); }; // assisted by forum user Klugier
	btnUpdateCustomer << [=] { btnUpdateCustomerClick(); };
	*/
	
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
	CustArray.NoRemoving();
	CustArray.SetOrderBy(CUST_ID);
	
	CustArray.Query();
	
	CustArray.WhenLeftDouble = [=] { EditRow(); };
	
}

void CustomersWindow::EditRow()
{
	int idNum;
	if(!CustArray.IsCursor())
		return;
	idNum = CustArray.GetKey();
	if (IsNull(idNum))
		return;
	AddCustomer dlg;
	dlg.Title("Edit Customer");

	//if(!dlg.ctrls.Load(CUSTOMERS, CUST_ID == idNum))
	//	return;
	SQL * Select(dlg.ctrls).From(CUSTOMERS).Where(CUST_ID == idNum);
	if(!dlg.ctrls.Fetch(SQL))
		return;
	if(dlg.Run() != IDOK)
		return;
	SQL * dlg.ctrls.Update(CUSTOMERS).Where(CUST_ID == idNum);
	CustArray.ReQuery();
	CustArray.FindSetCursor(idNum);
}
void CustomersWindow::AddNewCustomer()
{
    AddCustomer dlg;
    dlg.Title("New Customer");
    if(dlg.Execute() != IDOK)
        return;
    SQL * dlg.ctrls.Insert(CUSTOMERS);
    int id = SQL.GetInsertedId();
    CustArray.ReQuery();
    CustArray.FindSetCursor(id);
}
/*
void CustomersWindow::btnUpdateCustomerClick()
{
	PromptOK(__func__);
}

void CustomersWindow::btnSearchCustomerClick()
{
	PromptOK(__func__);
}
*/
