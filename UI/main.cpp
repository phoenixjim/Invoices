#include "Invoices.h"

struct ConvDoubleCls : Convert
{
	virtual Value Format ( const Value &q ) const;
};

Value ConvDoubleCls::Format ( const Value &q ) const
	{
		return q.IsNull() ? Null : UPP::Format ( "%.2f", q );
	}

Convert& ConvDouble()
{
	return Single<ConvDoubleCls>();
}

struct DateIntConvertCls : Convert {
    virtual Value Format(const Value& q) const;
    virtual Value Scan(const Value& text) const;
    virtual int Filter(int chr) const;
};

Value DateIntConvertCls::Format(const Value& q) const
{
    return IsNull(q) ? String() : ::Format(Date( 1970, 1, 1) + (int)q);
}

Value DateIntConvertCls::Scan(const Value& text) const
{
    String txt = text;
    if(IsNull(txt))
        return Null;
    txt.TrimLast(6);
    Date d;
    if(StrToDate(d, txt))
        return d - Date(1970, 1, 1);
    return ErrorValue("Invalid date!");
}

int DateIntConvertCls::Filter(int chr) const
{
    return CharFilterDate(chr);
}

Convert& DateIntConvert()
{
    return Single<DateIntConvertCls>();
}

void Invoices::MainMenu(Bar& bar)
{
	bar.Add(t_("Customers"),  [=]{ if(!cwin.IsOpen()) cwin.Open(this); });
	bar.Add(t_("Products"), [=]{ if(!prodwin.IsOpen()) prodwin.Open(this); });
	
	bar.Sub(t_("Transactions"), THISFN(TransactionsMenu));
	bar.Sub(t_("Reports"), THISFN(ReportsMenu));
	bar.Sub(t_("Management"), THISFN(ManagementMenu));
	bar.Add(t_("Exit"), THISFN(Close));
}

void Invoices::TransactionsMenu(Bar& bar)
{
	bar.Add(t_("Create Invoice"), [=]{ if(!createinvoicewin.IsOpen())
	createinvoicewin.Open(this); });
	bar.Add(t_("List Invoices"), [=]{ if(!listinvoiceswin.IsOpen())
	listinvoiceswin.Open(this); });
	bar.Add(t_("List Line Items"), [=]{ if(!listlineitemswin.IsOpen())
	listlineitemswin.Open(this); });
}

void Invoices::ReportsMenu(Bar& bar)
{
	bar.Add(t_("Income / Sales Tax"), [=]{ if(!taxreportwin.IsOpen()) taxreportwin.Open(this); });
	bar.Add(t_("Income / Sales Tax by Customer"), [=]{ if(!taxbycustomerwin.IsOpen()) taxbycustomerwin.Open(this); });
	bar.Add(t_("Income by Customer"), [=]{ if(!incomewin.IsOpen()) incomewin.Open(this); });
	bar.Add(t_("Profit / Loss"), [=]{ if(!profitwin.IsOpen()) profitwin.Open(this); });
}

void Invoices::ManagementMenu(Bar& bar)
{
	bar.Add(t_("Select Database File"), [=]{DBFile = SelectDB(); });
	bar.Add(t_("Show Current Database File"), [=]{PromptOK(DeQtf("Current Database file is: \n" + DBFile));});
	bar.Add(t_("Set Company Info"), [=]{if(!setcompanywin.IsOpen()) setcompanywin.Open(this); });
	bar.Add(t_("Select Output Directory"), [=]{OutputDirectory = GetOutputDirectory(); });
}

Invoices::Invoices()
{
	CtrlLayout(*this, "Invoices");
	AddFrame(mainmenu);
	mainmenu.Set(THISFN(MainMenu));
	/*
	from http://leonardoce.interfree.it/leowiki.html "simple configfile"
	*/
	if(FileExists(configfile))
	{
        VectorMap<String, String> cfg = LoadIniFile(configfile);
        DBFile = cfg.Get("DBFile", Null);
	}
	else {
		GetOutputDirectory();
		SelectDB();
	}
}

String Invoices::SelectDB()
{
	selectdbwin.Type(t_("Invoices DB"), "*.jts");
	if(!selectdbwin.ExecuteOpen(t_("Select DB File")))
	{
        return "";
	}
	/*
	from http://leonardoce.interfree.it/leowiki.html "simple configfile"
	*/
	cfg << "DBFile=" << selectdbwin.Get() << "\n";
	if(!SaveFile(ConfigFile(), cfg))
	{
    	Exclamation("Error saving configuration!");
	}
	return selectdbwin.Get();

}

String Invoices::GetOutputDirectory()
{
	if ( !selectodirwin.ExecuteSelectDir ( t_ ( "Choose output directory" ) ) )
	{
		return "";
	}

	cfg << "OutputDirectory=" << selectodirwin.Get() << "\n";

	if ( !SaveFile ( ConfigFile(), cfg ) )
	{
		Exclamation ( "Error saving configuration!" );
	}
	return selectodirwin.Get();
}

// Shortcut keys defined
/*
bool Invoices::Key(dword key, int count)
{
	if(key == K_CTRL_E && !ewin.IsOpen())
		ewin.Open(this);
	else
	if(key == K_CTRL_F && !fwin.IsOpen())
		fwin.Open(this);
	return false;
}
*/

GUI_APP_MAIN
{
	Invoices().Run();
}
