#include "conf.h"
//Written by Jan Dolinár aka dolik.rce on Ultimatepp.org forums

int config::Load(const char* fn){
	Clear();
	file=fn;
	String l;
	FileIn in(fn);
	if(!in) {LOG("Failed to open file"); return 1;}
	int c;
	for(;;){
		l=in.GetLine();
		while(l.EndsWith("|")||l.EndsWith("\\")){
			if(l.EndsWith("\\")){l.Trim(l.GetCount()-1);}
			l+="\n"+in.GetLine();
		}
		if(l.IsVoid()) break;
		if(l.StartsWith("#")){
			Add("#",l.Mid(1));
			continue;
		}
		c=l.FindFirstOf("#");
		if (c>=0){
			Add("@",l.Mid(c+1));
			l.Trim(c);
		}
		if (l.IsEmpty()) {
			Add("","");
			continue;
		}
		String k;
		int i;
		for(i=0;i<l.GetCount();i++) {
			if(l[i]=='=') break;
			if(l[i]!=' ') k.Cat(l[i]);
		}
		Add(k,l.Mid(i+1));
	}
	return 0;
}
void config::Set(String k,Value v){
	value[FindAdd(k)]=v.ToString();
}
Vector<String> config::GetLine(String k,int index){
	Vector<String> result;
	if(index>=GetCount(k)){return result;}
	String v=replace("|"+Get(k,""),"\\\n","");
	v=replace(v,"\n","");
	String s;
	int c=0;
	for(int i=0; i<v.GetCount(); i++){
		if(v[i]=='|'){
			if(++c==1+index){
				i++;
				for(;(v[i]!='|')&&(i<v.GetCount());i++){
					if((v[i]!=' ')&&(v[i]!='\t')){
						s.Cat(v[i]);
					} else {
						if(s!="") result.Add(s);
						s.Clear();
					}
				}
				if(s!="") result.Add(s);
				return result;
			}
		}
	}
	return result;
}
int config::GetCount(String k){
	String s=Get(k,"");
	if(s==""){return 0;}
	int c=1;
	for(int i=0; i<s.GetCount(); i++){
		if(s[i]=='|'){c++;}
	}
	return c;
}
Vector<Vector<String> > config::GetTable(String k){
	Vector<Vector<String> > result;
	result.SetCount(GetCount(k));
	for(int i=0; i<result.GetCount(); i++){
		result[i]=GetLine(k,i);
	}
	return result;
}
String config::ToString(){
	String result;
	String s;
	if(s!="") result.Cat("#"+s+"\n");
	for(int i=0; i<GetCount(); i++){
		if(!IsUnlinked(i)){
			if(key[i]=="@"){i++;}
			String val=replace(value[i],"\n","\\\n");
			val=replace(val,"|\\","|");
			result+=key[i]+(((key[i]=="#")||(key[i]==""))?"":"=")+val;
			if((i>0)&&(key[i-1]=="@")){result+="#"+value[i-1];}
			result+="\n";
		}
	}
	return result;
}

String config::replace(String str, String find, String repl){
	String ret;
	int lenStr = str.GetCount();
	int lenFind = find.GetCount();
	int i = 0, j;
	while ((j = str.Find(find, i)) >= i) {
		ret += str.Mid(i, j-i) + repl;
		i = j + lenFind;
		if (i >= lenStr)
			break;
	}
	ret += str.Mid(i);
	return ret;
}
