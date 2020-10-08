#include "configs.h"

void Configs::SelectDB()
{
	PromptOK("Need to hand-edit the file to use the 'real' DB");
	return;
	/*
	selectdbwin.Type(t_("Invoices DB"), "*.db");
	if(!selectdbwin.ExecuteOpen(t_("Select DB File")))
	{
        return;
	}
	String notice = selectdbwin.Get();
	cfg.Set("DBFile", selectdbwin.Get());
	SaveSettings();
	notice << "\n Please restart program to begin using this db";
	Exclamation(notice);
*/
}



void Configs::GetOutputDirectory()
{
	if ( !selectodirwin.ExecuteSelectDir ( t_ ( "Choose output directory" ) ) )
	{
		return;
	}

	String notice =	selectodirwin.Get() << " not yet implemented though, please hand edit config.";
	// cfg.Set("OutputDirectory", notice);
	// SaveSettings();
	return;
}

void Configs::CreateSample()
{
	// Sql sql;
	
}

void Configs::Populate()
{
	
	if (!FileExists(configfile))
	{
		cfg.Set("OutputDirectory","/home/james/Desktop/");
		cfg.Set("Companyname", "Company Name");
		cfg.Set("Companyowner", "James Lefavour");
		cfg.Set("Companyaddress", "Home");
		cfg.Set("Companycity", "My City");
		cfg.Set("Companystate", "My State");
		cfg.Set("Companyzip", "MyZip");
		cfg.Set("Companyphone", "(555) 555 - 1212");
		cfg.Set("Companyemail", "jim@myemail.com");
		if (!FileExists(ConfigFile("sample.db")))
			CreateSample();
		cfg.Set("DBFile", ConfigFile("sample.db"));
		
		SaveSettings();
	}
	OutputDirectory = cfg("OutputDirectory", "");
	companyname = cfg("Companyname", "");
	companyowner = cfg("Companyowne", "");
	companyaddress = cfg("Companyaddress", "");
	companycity = cfg("Companycity", "");;
	companystate = cfg("Companystate", "");
	companyzip = cfg("Companyzip", "");
	companyphone = cfg("Companyphone", "");
	companyemail = cfg("Companyemail", "");
	DBFile = cfg("DBFile", "");

}
Configs::Configs()
{
	configfile = ConfigFile("Invoices.ini");
	cfg.Load(configfile);
	Populate();
}

Configs::~Configs()
{
	SaveSettings();
}

void Configs::SaveSettings()
{
	if(!cfg.Save())
	{
    	Exclamation("Error saving configuration!");
	}

}
