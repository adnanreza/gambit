// File: elimdomd.h -- declarations for classes dealing with inspection and
// creation of dominated strategy supports
// $Id$
class ElimDomParamsDialog // Can not use MyDialogBox due to wxMULTIPLE
{
private:
	wxDialogBox *d;
	wxListBox 	*player_box;
	wxCheckBox	*all_box,*compress_box;
	wxRadioBox	*dom_type_box;
	bool 				all,compress;
	int					completed,num_players;
	int					dom_type;
	gArray<int>	players;
	// now come the private functions
	void 				EnablePlayerBox(Bool e)
	{((wxWindow *)player_box)->Enable(e);}
	void OnEvent(int result)
	{
	completed=result;
	if (completed==wxOK)
	{
		all=all_box->GetValue();compress=compress_box->GetValue();
		dom_type=dom_type_box->GetSelection();
		int num_selections,*selections=new int[num_players];
		num_selections=player_box->GetSelections(&selections);
		players=gArray<int>(num_selections);
		for (int i=1;i<=num_selections;i++) players[i]=selections[i-1]+1;
	}
	d->Show(FALSE);delete d;
	}
	static void ok_button_func(wxButton &ob,wxEvent &ev)
	{((ElimDomParamsDialog *)ob.GetClientData())->OnEvent(wxOK);}
	static void cancel_button_func(wxButton &ob,wxEvent &ev)
	{((ElimDomParamsDialog *)ob.GetClientData())->OnEvent(wxCANCEL);}
	static void help_button_func(wxButton &ob,wxEvent &ev)
	{wxHelpContents(NFG_ELIMDOM_HELP);}

public:
	ElimDomParamsDialog(int numplayers,wxWindow *parent=NULL)
	{
	num_players=numplayers;
	all=FALSE,compress=FALSE;
	d=new wxDialogBox(parent,"ElimDom Parameters",TRUE);
	all_box=new wxCheckBox(d,0,"Find All");
	compress_box=new wxCheckBox(d,NULL,"Compress");
	d->NewLine();
	char *dom_type_list[2]={"Weak","Strong"};
	dom_type_box=new wxRadioBox(d,NULL,"Dom type",-1,-1,-1,-1,2,dom_type_list,2);
	wxStringList *player_list=wxStringListInts(num_players);
	player_box=new wxListBox(d,NULL,"Players",wxMULTIPLE,-1,-1,-1,-1,num_players,
		player_list->ListToArray());
	for (int i=1;i<=num_players;i++) player_box->SetSelection(i-1,TRUE);
	d->NewLine();

	wxButton *ok_button=new wxButton(d,(wxFunction)ok_button_func,"Ok");
	ok_button->SetClientData((char *)this);
	wxButton *cancel_button=new wxButton(d,(wxFunction)cancel_button_func,"Cancel");
	cancel_button->SetClientData((char *)this);
	wxButton *help_button=new wxButton(d,(wxFunction)help_button_func,"?");
	d->Fit();
	d->Show(TRUE);
	}
	// Data access functions
	gArray<int> Players(void) {return players;}
	bool FindAll(void) {return all;}
	bool Compress(void) {return compress;}
	bool DomStrong(void) {return dom_type;}
	int  Completed(void) {return completed;}
};

class SupportInspectDialog:public wxDialogBox
{
private:
	BaseNormShow *bns;
	const gList<NFSupport *> &sups;
	wxText *cur_dim,*disp_dim;
	wxChoice *disp_item,*cur_item;
	char *cur_str,*disp_str;
	wxStringList *support_list;
// Low level event handlers
	static void cur_func(wxChoice &ob,wxEvent &ev)
	{((SupportInspectDialog *)ob.GetClientData())->OnCur(ob.GetSelection()+1);}
	static void disp_func(wxChoice &ob,wxEvent &ev)
	{((SupportInspectDialog *)ob.GetClientData())->OnDisp(ob.GetSelection()+1);}
	static void new_sup_func(wxButton &ob,wxEvent &ev)
	{((SupportInspectDialog *)ob.GetClientData())->OnNewSupport();}
	static void change_sup_func(wxButton &ob,wxEvent &ev)
	{((BaseNormShow *)ob.GetClientData())->SupportInspect(SUPPORT_CHANGE);}
	static void help_func(wxButton &ob,wxEvent &ev)
	{wxHelpContents(NFG_SUPPORTS_HELP);}
	static void close_func(wxButton &ob,wxEvent &ev)
	{((BaseNormShow *)ob.GetClientData())->SupportInspect(SUPPORT_CLOSE);}
// High level event handlers
	void OnNewSupport(void)
	{
	if (bns->MakeSupport())
	{
		disp_item->Append(ToString(sups.Length()));
		disp_item->SetSize(-1,-1,-1,-1);
		cur_item->Append(ToString(sups.Length()));
		cur_item->SetSize(-1,-1,-1,-1);
	}
	}
	void OnCur(int cur_sup);
	void OnDisp(int disp_sup)
	{disp_dim->SetValue(array_to_string(sups[disp_sup]->SupportDimensions()));}

// Utility funcs
	static gString array_to_string(const gArray<int> &a)
	{
	gString tmp='(';
	for (int i=1;i<=a.Length();i++) tmp+=ToString(a[i])+((i==a.Length()) ? ")" : ",");
	return tmp;
	}
public:
	SupportInspectDialog(const gList<NFSupport *> &sups_,int cur_sup,int disp_sup,BaseNormShow *bns_,wxWindow *parent=0)
		: wxDialogBox(parent,"Supports"),sups(sups_),bns(bns_)
	{
	wxForm *f=new wxForm(0);
	SetLabelPosition(wxVERTICAL);
	cur_dim=new wxText(this,0,"Current",
											array_to_string(sups[cur_sup]->SupportDimensions()),
											-1,-1,80,-1,wxREADONLY);
	disp_dim=new wxText(this,0,"Display",
											array_to_string(sups[disp_sup]->SupportDimensions()),
											-1,-1,80,-1,wxREADONLY);
	support_list=wxStringListInts(sups.Length());
	cur_str=new char[10];strcpy(cur_str,ToString(cur_sup));
	disp_str=new char[10];strcpy(disp_str,ToString(disp_sup));
	wxFormItem *cur_fitem=wxMakeFormString("",&cur_str,wxFORM_CHOICE,
					new wxList(wxMakeConstraintStrings(support_list),0));
	f->Add(cur_fitem);
	f->Add(wxMakeFormMessage("      ")); // fix the spacing... not neat but..
	wxFormItem *disp_fitem=wxMakeFormString("",&disp_str,wxFORM_CHOICE,
					new wxList(wxMakeConstraintStrings(support_list),0));
	f->Add(disp_fitem);
	f->Add(wxMakeFormNewLine());
	wxFormItem *close_fitem=wxMakeFormButton("Close",(wxFunction)close_func);
	f->Add(close_fitem);
	wxFormItem *cngsup_fitem=wxMakeFormButton("Apply",(wxFunction)change_sup_func);
	f->Add(cngsup_fitem);
	wxFormItem *newsup_fitem=wxMakeFormButton("New",(wxFunction)new_sup_func);
	f->Add(newsup_fitem);
	wxFormItem *help_fitem=wxMakeFormButton("?",(wxFunction)help_func);
	f->Add(help_fitem);
	f->AssociatePanel(this);
	cur_item=(wxChoice *)cur_fitem->GetPanelItem();
	cur_item->Callback((wxFunction)cur_func);
	cur_item->SetClientData((char *)this);
	disp_item=(wxChoice *)disp_fitem->GetPanelItem();
	disp_item->SetClientData((char *)this);
	disp_item->Callback((wxFunction)disp_func);
	((wxButton *)newsup_fitem->GetPanelItem())->SetClientData((char *)this);
	((wxButton *)cngsup_fitem->GetPanelItem())->SetClientData((char *)bns);
	((wxButton *)help_fitem->GetPanelItem())->SetClientData((char *)this);
	((wxButton *)close_fitem->GetPanelItem())->SetClientData((char *)bns);
  Fit();
	Show(TRUE);
	}
	// Data Access members
	int 	CurSup(void) {return cur_item->GetSelection()+1;}
	int 	DispSup(void) {return disp_item->GetSelection()+1;}
};


void SupportInspectDialog::OnCur(int cur_sup)
{
cur_dim->SetValue(array_to_string(sups[cur_sup]->SupportDimensions()));
disp_dim->SetValue(array_to_string(sups[cur_sup]->SupportDimensions()));
disp_item->SetSelection(cur_sup-1);
}

