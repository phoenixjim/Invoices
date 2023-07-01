#include "configs.h"

void MyData::Jsonize ( JsonIO& json )

{
	json
		("TaxRate", taxrate)
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
	String cfg = GetExeFilePath();
	// cfg << ".json"; //"\" << ConfigFile( GetExeTitle() + ".json" );
	
	#ifdef PLATFORM_WIN32
	cfg << "-W.json";
	#else
	cfg << "-L.json";
	#endif
	
	if (!FileExists ( cfg ) )
	// if ( !FileExists ( ConfigFile(GetExeTitle() + ".json") ) )
	{
		data.taxrate = 0.08;
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
	String cfg = GetExeFilePath();
	#ifdef PLATFORM_WIN32
	cfg << "-W.json";
	#else
	cfg << "-L.json";
	#endif
	
	Populate();
	LoadFromJsonFile ( data,  cfg );
}

void Configs::SaveSettings()
{
	// force config file to CWD
	String cfg = GetExeFilePath();
	#ifdef PLATFORM_WIN32
	cfg << "-W.json";
	#else
	cfg << "-L.json";
	#endif
	
	StoreAsJsonFile( data, cfg );
}
