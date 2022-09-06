#include "DBUI.h"
#include "../Utils/configs.h"

class AddType : public WithTypesAddLayout<TopWindow> {
	public:
		SqlCtrls ctrls;
		AddType(String title);
		virtual void Paint(Draw& w) {
			w.DrawRect(GetSize(), COLOR); // <= enter your background color here
    }
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
	lName.SetInk(TXTCOLOR);
	lDescript.SetInk(TXTCOLOR);
	lQuant.SetInk(TXTCOLOR);
	lCost.SetInk(TXTCOLOR);
	lType.SetInk(TXTCOLOR);
	lInstruct.SetInk(TXTCOLOR);
	optTaxable.SetColor(TXTCOLOR);
}

TypesWindow::TypesWindow() {
	CtrlLayout(*this, "Types");	
	btnAddType << [=] { AddNewType(); };

	TypeArray.SetTable(TYPES, TYPENUM);
		
	TypeArray.AddColumn(TYPENAME, "Name");
	TypeArray.AddColumn(TYPEDESCR, "Description");
	TypeArray.AddColumn(TYPECOST, "Cost").SetConvert(ConvCurrency()).SetDisplay ( StdRightDisplay() ).HeaderTab().AlignRight();
	TypeArray.AddColumn(TYPEQUANT, "Quantity").SetDisplay ( StdCenterDisplay() ).HeaderTab().AlignCenter();
	TypeArray.AddColumn(TYPETAXABLE, "Tax?").SetDisplay ( StdCenterDisplay() ).HeaderTab().AlignCenter();
	TypeArray.ColumnWidths("65 135 40 20 20");
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
