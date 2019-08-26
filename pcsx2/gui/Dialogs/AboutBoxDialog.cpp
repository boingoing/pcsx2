/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2010  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PrecompiledHeader.h"
#include "App.h"
#include "AppCommon.h"

#include "Dialogs/ModalPopups.h"

#include "Utilities/EmbeddedImage.h"
#include "Resources/Logo.h"

#include <wx/mstream.h>
#include <wx/hyperlink.h>

using namespace pxSizerFlags;

// --------------------------------------------------------------------------------------
//  AboutBoxDialog  Implementation
// --------------------------------------------------------------------------------------

Dialogs::AboutBoxDialog::AboutBoxDialog(wxWindow* parent)
	: wxDialogWithHelpers(parent, AddAppName(_("About %s")), pxDialogFlags())
	, m_bitmap_logo(this, wxID_ANY, wxBitmap(EmbeddedImage<res_Logo>().Get()),
		wxDefaultPosition, wxDefaultSize
		)
{
	// Main layout

	SetMinWidth(450);

	*this += m_bitmap_logo | StdCenter();

	*this += Text(_("PlayStation 2 Emulator"));

	*this += new wxHyperlinkCtrl(this, wxID_ANY,
		_("Main Website"), L"https://pcsx2.net"
		) | pxProportion(1).Center().Border(wxALL, 3);

	*this += new wxHyperlinkCtrl(this, wxID_ANY,
		_("Support Forums"), L"https://forums.pcsx2.net"
	) | pxProportion(1).Center().Border(wxALL, 3);

	*this += new wxHyperlinkCtrl(this, wxID_ANY,
		_("Git Repository at GitHub"), L"https://github.com/PCSX2/pcsx2"
		) | pxProportion(1).Center().Border(wxALL, 3);

	*this += new wxHyperlinkCtrl(this, wxID_ANY,
		_("Contributors"), L"https://github.com/PCSX2/pcsx2/graphs/contributors"
	) | pxProportion(1).Center().Border(wxALL, 3);

	*this += new wxHyperlinkCtrl(this, wxID_ANY,
		_("License"), L"https://github.com/PCSX2/pcsx2/blob/master/COPYING.GPLv2"
	) | pxProportion(2).Center().Border(wxALL, 3);

	*this += Text(_("Social media links:"));

	*this += new wxHyperlinkCtrl(this, wxID_ANY,
		_("Facebook Page"), L"https://www.facebook.com/Pcsx2-145051045565631"
		) | pxProportion(1).Center().Border(wxALL, 3);

	*this += new wxHyperlinkCtrl(this, wxID_ANY,
		_("Facebook Group"), L"https://www.facebook.com/groups/98483509559"
	) | pxProportion(1).Center().Border(wxALL, 3);

	*this += new wxHyperlinkCtrl(this, wxID_ANY,
		_("Twitter Page"), L"https://twitter.com/PCSX2"
		) | pxProportion(1).Center().Border(wxALL, 3);

	*this += new wxHyperlinkCtrl(this, wxID_ANY,
		_("Youtube Channel"), L"https://www.youtube.com/channel/UC-gDRnHdVMrgUndopJgtVSA"
		) | pxProportion(2).Center().Border(wxALL, 3);

	wxButton& closeButton = *new wxButton(this, wxID_OK, _("Close"));
	closeButton.SetFocus();
	*this += closeButton | StdCenter();

	SetSizerAndFit(GetSizer());
}
