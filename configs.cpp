#include "configs.h"

String Configs::SelectDB()
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

String Configs::GetOutputDirectory()
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

