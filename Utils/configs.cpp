#include "configs.h"

void MyData::Jsonize ( JsonIO& json )

{
	json
		("DBFile", dbfile)
		("CompanyName", companyname)
		("CompanyOwner", companyowner)
		("CompanyAddress", companyaddress)
		("CompanyCity", companycity)
		("CompanyState", companystate)
		("CompanyZip", companyzip)
		("CompanyPhone", companyphone)
		("CompanyEmail", companyemail)
		;
}

void Configs::SelectDB()
{
	selectdbwin.Type(t_("Invoices DB"), "*.db");
	if(!selectdbwin.ExecuteOpen(t_("Select DB File")))
	{
	    return;
	}
	data.dbfile = selectdbwin.Get();
	SaveSettings();
}

void Configs::Populate()
{
	// force config file to CWD
	
	// cfg << ".json"; //"\" << ConfigFile( GetExeTitle() + ".json" );
	
	#ifdef PLATFORM_WIN32
	String cfg = GetCurrentDirectory() << "Invoices-W.json";
	#else
	String cfg = GetExeFilePath() << "-L.json";
	#endif
	
	if (!FileExists ( cfg ) )
	// if ( !FileExists ( ConfigFile(GetExeTitle() + ".json") ) )
	{
		data.companyname = "Company Name";
		data.companyowner = "James Lefavour";
		data.companyaddress = "0 Main Way";
		data.companycity = "City";
		data.companystate = "State";
		data.companyzip = "00000";
		data.companyphone = "(555) 555 - 1212";
		data.companyemail = "jim@myemail.com";
		SelectDB();
	}
}

Configs::Configs()
{
	// force config file to CWD
	#ifdef PLATFORM_WIN32
	String cfg = GetCurrentDirectory() << "Invoices-W.json";
	#else
	String cfg = GetExeFilePath() << "-L.json";
	#endif
	
	Populate();
	LoadFromJsonFile ( data,  cfg );
}

void Configs::SaveSettings()
{
	// force config file to CWD
	#ifdef PLATFORM_WIN32
	String cfg = GetCurrentDirectory() << "Invoices-W.json";
	#else
	String cfg = GetExeFilePath() << "-L.json";
	#endif
	
	StoreAsJsonFile( data, cfg );
}
