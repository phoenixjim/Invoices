#include "configs.h"

void Configs::SelectDB()
{
	selectdbwin.Type(t_("Invoices DB"), "*.db");
	if(!selectdbwin.ExecuteOpen(t_("Select DB File")))
	{
        return;
	}
	/*
	from http://leonardoce.interfree.it/leowiki.html "simple configfile"
	*/
	String notice = selectdbwin.Get();
	Sqlite3Session sqlite3;
	
	if(!sqlite3.Open(notice)) {
		Exclamation("Can't create or open database file\n");
		return;
	}

	SQL = sqlite3;
	

	notice << " not yet implemented though, please hand edit config.";
	Exclamation(notice);
	return;

}

String Configs::GetOutputDirectory()
{
	if ( !selectodirwin.ExecuteSelectDir ( t_ ( "Choose output directory" ) ) )
	{
		return "";
	}

	String notice =	selectodirwin.Get() << " not yet implemented though, please hand edit config.";

	SaveSettings();
	return selectodirwin.Get();
}

Configs::Configs()
{
	configfile = ConfigFile("Invoices.ini");

	if (!FileExists(configfile))
	{
		cfg = "OutputDirectory=/home/james/Desktop/\n";
		cfg << "Companyname=Company Name\n";
		cfg << "Companyowner=James Lefavour\n";
		cfg << "Companyaddress=Home\n";
		cfg << "Companycity=My City\n";
		cfg << "Companystate=My State\n";
		cfg << "Companyzip=MyZip\n";
		cfg << "Companyphone=(555) 555 - 1212\n";
		cfg << "Companyemail=jim@myemail.com\n";
		cfg << "DBFile=/home/james/upp/MyApps/Invoices/sample.db\n";
		
		SaveSettings();
	}
	VectorMap<String, String> mycfg = LoadIniFile(configfile);
	OutputDirectory = mycfg.Get("OutputDirectory", Null);
	companyname = mycfg.Get("Companyname", Null);
	companyowner = mycfg.Get("Companyowne", Null);
	companyaddress = mycfg.Get("Companyaddress", Null);
	companycity = mycfg.Get("Companycity", Null);;
	companystate = mycfg.Get("Companystate", Null);
	companyzip = mycfg.Get("Companyzip", Null);
	companyphone = mycfg.Get("Companyphone", Null);
	companyemail = mycfg.Get("Companyemail", Null);
	DBFile = mycfg.Get("DBFile", Null);
}

void Configs::SaveSettings()
{
	if(!SaveFile(configfile, cfg))
	{
    	Exclamation("Error saving configuration!");
	}

}
