#ifndef _Invoices_setcompany_h_
#define _Invoices_setcompany_h_
struct SetCompanyWindow : WithSetCompanyWindowLayout<TopWindow> {
public:
	SetCompanyWindow() { CtrlLayout(*this, "Set Company Info"); }
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};


#endif
