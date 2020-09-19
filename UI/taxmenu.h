#ifndef _Invoices_taxmenu_h_
#define _Invoices_taxmenu_h_
struct TaxMenuWindow : WithTaxWindowLayout<TopWindow> {
public:
	TaxMenuWindow() { CtrlLayout(*this, "Income / Sales Tax"); }
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};


#endif
