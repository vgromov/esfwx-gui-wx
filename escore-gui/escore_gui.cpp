#include "escore_gui_pch.h"
#pragma hdrstop

USEFORM("TDlgUserPrompt.cpp", DlgUserPrompt);
USEFORM("TFmeAppLog.cpp", FmeAppLog); /* TFrame: File Type */
USEFORM("TDlgBase.cpp", DlgBase);
USEFORM("TDlgBrowseForFolder.cpp", DlgBrowseForFolder);
USEFORM("TFmeProgressMonitorView.cpp", FmeProgressMonitorView); /* TFrame: File Type */
USEFORM("TFrmReflectedClassConfig.cpp", FrmReflectedClassConfig); /* TFrame: File Type */
USEFORM("TFmeBrowseForFolder.cpp", FmeBrowseForFolder); /* TFrame: File Type */
//---------------------------------------------------------------------------
#pragma package(smart_init)
#include "autolink.cxx"

//---------------------------------------------------------------------------
//   Package source.
//---------------------------------------------------------------------------

#pragma argsused
extern "C" int _libmain(unsigned long reason)
{
	return 1;
}
//---------------------------------------------------------------------------
