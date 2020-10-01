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
		
	CustArray.AddColumn(CUSTNAME, "Name");
	CustArray.AddColumn(EMAIL, "Email");
	CustArray.AddColumn(CONTACT, "Phone");
	CustArray.AddColumn(ADDRESS, "Address");
	CustArray.AddColumn(CITY, "City");
	CustArray.AddColumn(STATE, "State");
	CustArray.AddColumn(ZIP, "Zip");
	CustArray.AddColumn(TAXABLE, "Taxable?");
	CustArray.ColumnWidths("40 40 20 50 20 15 10 5");
	CustArray.Appending().NoRemoving();
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
