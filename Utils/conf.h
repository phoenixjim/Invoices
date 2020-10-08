#ifndef _nanopdf_config_h_
#define _nanopdf_config_h_
#include <Core/Core.h>
//Written by Jan Dolinár aka dolik.rce on Ultimatepp.org forums

using namespace Upp;

class config: public VectorMap<String, String>{
public:
	String file;
	config()												{file=String::GetVoid();}
	config(const char* fn)									{file=fn; Load(fn);}
	bool Save()												{return SaveFile(file,ToString());}
	int GetCount()											{return VectorMap<String,String>::GetCount();}
	void Set(String k,Value v);
	template<class T> void SetG(String k,const T& g);
	template<class T> void FillG(String k,T& g,String scheme);
	int Load(const char* fn);
	Vector<String> GetLine(String k,int index);
	int GetCount(String k);
	Vector<Vector<String> > GetTable(String k);
	String ToString();
	String operator()(const char* var,const char* def="")	{return Get(var,def);}
private:
	String replace(String str, String find, String replace);
};

template<class T> 
void config::SetG(String k,const T& g){
	if(g.GetCount()==0){return;};
	String str="\n";
	for(int i=0; i<g.GetCount(); i++){
		for(int j=0; j<g.GetColumnCount(); j++){
			str+=AsString(g.Get(i,j))+"\t";
		}
		str+="|\n";
	}
	str.Trim(str.GetCount()-2);
	value[FindAdd(k)]=str;
}

template<class T>
void config::FillG(String k,T& g,String scheme){
	Vector<Vector<String> > tbl=GetTable(k);
	g.Clear();
	for(int i=0; i<tbl.GetCount(); i++){
		g.Add();
		for(int j=0; j<min(tbl[i].GetCount(),g.GetColumnCount()); j++){
			switch(scheme[j]) {
				case 's': g.Set(i,j,tbl[i][j]); break;
				case 'd': g.Set(i,j,StrDbl(tbl[i][j])); break;
				case 'i': g.Set(i,j,StrInt(tbl[i][j])); break;
			}
		}
	}
}

#endif
