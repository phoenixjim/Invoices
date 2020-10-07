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
	configfile = ConfigFile();
	if (!FileExists(configfile))
	{
		DBFile = cfg = "DBFile=/home/james/upp/MyApps/Invoices/sample.db";
		cfg << "\n";
		OutputDirectory = cfg << "OutputDirectory=/home/james/Desktop/";
		cfg << "\n";
		companyname = cfg << "companyname=Company Name";
		cfg << "\n";
		companyowner = cfg << "companyowner=James Lefavour";
		cfg << "\n";
		companyaddress = cfg << "companyaddress=Home";
		cfg << "\n";
		companycity = cfg << "companycity=My City";
		cfg << "\n";
		companystate = cfg << "companystate=My State";
		cfg << "\n";
		companyzip = cfg << "companyzip=MyZip";
		cfg << "\n";
		companyphone = cfg << "companyphone=(555) 555 - 1212";
		cfg << "\n";
		companyemail = cfg << "companyemail=jim@myemail.com";
		cfg << "\n";
		
		SaveSettings();
	}
	else
	{
		VectorMap<String, String> cfg = LoadIniFile(configfile);
		DBFile = cfg.Get("DBFile", Null);
		OutputDirectory = cfg.Get("OutputDirectory", Null);
		companyname = cfg.Get("companyname", Null);
		companyowner = cfg.Get("companyowne", Null);
		companyaddress = cfg.Get("companyaddress", Null);
		companycity = cfg.Get("companycity", Null);;
		companystate = cfg.Get("companystate", Null);
		companyzip = cfg.Get("companyzip", Null);
		companyphone = cfg.Get("companyphone", Null);
		companyemail = cfg.Get("companyemail", Null);
	}
	// DBFile = "/home/james/OneDrive/JTS/Jim's Tech Services/invoices.db";

	/*
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
*/
}

void Configs::SaveSettings()
{
	if(!SaveFile(ConfigFile(), cfg))
	{
    	Exclamation("Error saving configuration!");
	}

}
