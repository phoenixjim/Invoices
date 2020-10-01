#include "configs.h"

String Configs::SelectDB()
{
	selectdbwin.Type(t_("Invoices DB"), "*.db");
	if(!selectdbwin.ExecuteOpen(t_("Select DB File")))
	{
        return "";
	}
	/*
	from http://leonardoce.interfree.it/leowiki.html "simple configfile"
	*/
	cfg << "DBFile=" << selectdbwin.Get() << "\n";
	SaveSettings();
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

void Configs::Initialize()
{
	if (DBFile.IsEmpty()) DBFile = SelectDB();
	
	if (OutputDirectory.IsEmpty()) OutputDirectory = GetOutputDirectory();
	if (companyname.IsEmpty()) {
		cfg << "DBFile" << DBFile;
		cfg << "OutputDirectory" << OutputDirectory;
		cfg << "companyname=";
		cfg << "companyowner=";
		cfg << "companyaddress=";
		cfg << "companycity=";
		cfg << "companystate=";
		cfg << "companyzip=";
		cfg << "companyphone=";
		cfg << "companyemail=";
	}
	SaveSettings();
	// more configs here
}

void Configs::SaveSettings()
{
	if(!SaveFile(ConfigFile(), cfg))
	{
    	Exclamation("Error saving configuration!");
	}
}
