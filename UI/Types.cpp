#include "DBUI.h"
#include "../Utils/configs.h"

class AddType : public WithTypesAddLayout<TopWindow> {
	public:
		SqlCtrls ctrls;
		AddType(String title);
};


AddType::AddType(String title)
{
	CtrlLayoutOKCancel(*this, title);
	ctrls
		(TYPENUM, txtTypeNo)
		(TYPENAME, txtTypeName)
		(TYPEDESCR, txtDescription)
		(TYPECOST, txtTypeCost)
		(TYPEQUANT, txtTypeQuantity)
		(TYPETAXABLE, optTaxable)
		;
}

TypesWindow::TypesWindow() {
	CtrlLayout(*this, "Types");	
	btnAddType << [=] { AddNewType(); };

	TypeArray.SetTable(TYPES, TYPENUM);
		
	TypeArray.AddColumn(TYPENAME, "Name");
	TypeArray.AddColumn(TYPEDESCR, "Description");
	TypeArray.AddColumn(TYPECOST, "Cost").SetConvert(ConvDouble());
	TypeArray.AddColumn(TYPEQUANT, "Quantity");
	TypeArray.AddColumn(TYPETAXABLE, "Tax?");
	TypeArray.ColumnWidths("100 100 40 30 30");
	TypeArray.SetOrderBy(TYPENUM).Appending().Removing();
	TypeArray.Query();

	TypeArray.WhenLeftDouble = [=] { EditRow();};
}

void TypesWindow::EditRow() // CONTINUE HERE
{
	int idNum;
	if(!TypeArray.IsCursor())
		return;
	idNum = TypeArray.GetKey();
	if (IsNull(idNum))
		return;
	AddType dlg("Edit Type");
	
	SQL * Select(dlg.ctrls).From(TYPES).Where(TYPENUM == idNum);
	if(!dlg.ctrls.Fetch(SQL))
		return;
	if(dlg.Run() != IDOK)
		return;
	SQL * dlg.ctrls.Update(TYPES).Where(TYPENUM == idNum);
	TypeArray.ReQuery();
	TypeArray.FindSetCursor(idNum);
}

void TypesWindow::AddNewType()
{
	AddType dlg("Add Type");
	if(dlg.Run() != IDOK)
		return;
	SQL * dlg.ctrls.Insert(TYPES);

    int id = SQL.GetInsertedId();

    TypeArray.ReQuery();
    TypeArray.FindSetCursor(id);
}
