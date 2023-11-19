#include "DBUI.h"
#include "../Utils/configs.h"

class AddCounty : public WithCountiesAddLayout<TopWindow> {
	public:
		SqlCtrls ctrls;
		AddCounty(String title);
		virtual void Paint(Draw& w) {
			w.DrawRect(GetSize(), COLOR); // <= enter your background color here
    }
};


AddCounty::AddCounty(String title)
{
	CtrlLayoutOKCancel(*this, title);
	ctrls
		(COUNTY_NUM, txtCountyNo)
		(COUNTY_NAME, txtCountyName)
		(COUNTY_RATE, txtTaxRate)
		;
	lName.SetInk(TXTCOLOR);
	lRate.SetInk(TXTCOLOR);
	lNumber.SetInk(TXTCOLOR);
}

CountiesWindow::CountiesWindow() {
	CtrlLayout(*this, "Counties");	
	MinimizeBox();
	btnAddCounty << [=] { AddNewCounty(); };

	CountyArray.SetTable(COUNTIES, COUNTY_NUM);
		
	CountyArray.AddColumn(COUNTY_NUM, "No:");
	CountyArray.AddColumn(COUNTY_NAME, "Name");
	CountyArray.AddColumn(COUNTY_RATE, "Rate").SetConvert(ConvRate()).SetDisplay ( StdCenterDisplay() ).HeaderTab().AlignCenter();
	CountyArray.ColumnWidths("20 135 20");
	CountyArray.SetOrderBy(COUNTY_NUM).Appending().Removing();
	CountyArray.Query();

	CountyArray.WhenLeftDouble = [=] { EditRow();};
}

void CountiesWindow::EditRow() // CONTINUE HERE
{
	int idNum;
	if(!CountyArray.IsCursor())
		return;
	idNum = CountyArray.GetKey();
	if (IsNull(idNum))
		return;
	AddCounty dlg("Edit County_");
	
	SQL * Select(dlg.ctrls).From(COUNTIES).Where(COUNTY_NUM == idNum);
	if(!dlg.ctrls.Fetch(SQL))
		return;
	if(dlg.Run() != IDOK)
		return;
	SQL * dlg.ctrls.Update(COUNTIES).Where(COUNTY_NUM == idNum);
	CountyArray.ReQuery();
	CountyArray.FindSetCursor(idNum);
}

void CountiesWindow::AddNewCounty()
{
	AddCounty dlg("Add County");
	if(dlg.Run() != IDOK)
		return;
	SQL * dlg.ctrls.Insert(COUNTIES);

    int id = SQL.GetInsertedId();

    CountyArray.ReQuery();
    CountyArray.FindSetCursor(id);
}
