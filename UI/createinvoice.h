#ifndef _Invoices_createinvoice_h_
#define _Invoices_createinvoice_h_
struct CreateInvoiceWindow : WithCreateInvoiceWindowLayout<TopWindow> {
public:
	CreateInvoiceWindow() { CtrlLayout(*this, "Create Invoice"); }
	void Paint(Draw& w) {
        w.DrawRect(GetSize(), Color(204, 255, 255)); // <= enter your background color here
     }
};


#endif
