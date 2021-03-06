/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_scui.hxx"


//------------------------------------------------------------------

#include "tabbgcolordlg.hxx"
#include "scresid.hxx"
#include "miscdlgs.hrc"

#include <tools/debug.hxx>
#include <tools/color.hxx>
#include <sfx2/objsh.hxx>
#include <svx/xtable.hxx>
#include <svx/drawitem.hxx>
#include <unotools/pathoptions.hxx>
#include <tools/resid.hxx>
#include <editeng/editrids.hrc>
#include <editeng/eerdll.hxx>

#include <boost/scoped_ptr.hpp>

//==================================================================

#define HDL(hdl) LINK(this,ScTabBgColorDlg,hdl)

ScTabBgColorDlg::ScTabBgColorDlg( Window*         pParent,
                                  const String&   rTitle,
                                  const String&   rTabBgColorNoColorText,
                                  const Color&    rDefaultColor,
                                  const rtl::OString& sHelpId ) :
    ModalDialog    ( pParent, ScResId( RID_SCDLG_TAB_BG_COLOR ) ),
    aBorderWin              ( this, ScResId( TAB_BG_COLOR_CT_BORDER ) ),
    aTabBgColorSet          ( &aBorderWin, ScResId( TAB_BG_COLOR_SET_BGDCOLOR ), this ),
    aBtnOk                  ( this, ScResId( BTN_OK ) ),
    aBtnCancel              ( this, ScResId( BTN_CANCEL ) ),
    aBtnHelp                ( this, ScResId( BTN_HELP ) ),
    aTabBgColor             ( rDefaultColor ),
    aTabBgColorNoColorText  ( rTabBgColorNoColorText ),
    msHelpId                ( sHelpId )

{
    SetHelpId( sHelpId );
    this->SetText( rTitle );
    this->SetStyle(GetStyle() | WB_BORDER | WB_STDFLOATWIN | WB_3DLOOK | WB_DIALOGCONTROL | WB_SYSTEMWINDOW | WB_STANDALONE | WB_HIDE);

    FillColorValueSets_Impl();
    aTabBgColorSet.SetDoubleClickHdl( HDL(TabBgColorDblClickHdl_Impl) );
    aBtnOk.SetClickHdl( HDL(TabBgColorOKHdl_Impl) );
    FreeResource();
}

//------------------------------------------------------------------------

void ScTabBgColorDlg::GetSelectedColor( Color& rColor ) const
{
    rColor = this->aTabBgColor;
}

ScTabBgColorDlg::~ScTabBgColorDlg()
{
}

void ScTabBgColorDlg::FillColorValueSets_Impl()
{
    SfxObjectShell* pDocSh = SfxObjectShell::Current();
    const SfxPoolItem* pItem = NULL;
    XColorListSharedPtr aColorTable;
    const Size aSize15x15 = Size( 15, 15 );
    const Size aSize10x10 = Size( 10, 10 );
    const Size aSize5x5 = Size( 5, 5 );
    sal_uInt16 nSelectedItem = 0;

    DBG_ASSERT( pDocSh, "DocShell not found!" );

    if ( pDocSh && ( 0 != ( pItem = pDocSh->GetItem(SID_COLOR_TABLE) ) ) )
    {
        aColorTable = static_cast< const SvxColorTableItem* >(pItem)->GetColorTable();
    }

    if ( !aColorTable.get() )
    {
        aColorTable = XPropertyListFactory::CreateSharedXColorList(SvtPathOptions().GetPalettePath());
    }
    
    long nColorCount(0);

    if ( aColorTable.get() )
    {
        nColorCount = aColorTable->Count();
        Color aColWhite( COL_WHITE );
        String aStrWhite( EditResId( RID_SVXITEMS_COLOR_WHITE ) );

        aTabBgColorSet.addEntriesForXColorList(aColorTable);
    }

    if(nColorCount)
    {
        const WinBits nBits(aTabBgColorSet.GetStyle() | WB_NAMEFIELD | WB_ITEMBORDER | WB_NONEFIELD | WB_3DLOOK | WB_NO_DIRECTSELECT | WB_NOPOINTERFOCUS);
        aTabBgColorSet.SetText( aTabBgColorNoColorText );
        aTabBgColorSet.SetStyle( nBits );
        static sal_Int32 nAdd = 4;

        // calculate new size of color control as base, derive size of border win
        const Size aNewSize(aTabBgColorSet.layoutAllVisible(nColorCount));
        const Size aNewSizeBorderWin(aNewSize.Width() + nAdd, aNewSize.Height() + nAdd);

        // from that, calculate a new dialog size
        const Size aCurrentSizeDialog(GetOutputSizePixel());
        const Size aCurrentSizeBorderWin(aBorderWin.GetOutputSizePixel());
        const long nOffsetX(aCurrentSizeDialog.Width() - aCurrentSizeBorderWin.Width());
        const long nOffsetY(aCurrentSizeDialog.Height() - aCurrentSizeBorderWin.Height());
        const Size aNewSizeDialog(aNewSizeBorderWin.Width() + nOffsetX, aNewSizeBorderWin.Height() + nOffsetY);

        // also need to adapt pos and size for the three buttons; as a base, take their original
        // distance from the dialog bottom and get new Y-Pos
        const long aButtonOffsetFromBottom(aCurrentSizeDialog.Height() - aBtnOk.GetPosPixel().Y());
        const long aNewButtonY(aNewSizeDialog.Height() - aButtonOffsetFromBottom);

        // for each button, scale width and x-pos by old/new dialog sizes and re-layout
        // for Okay-Button
        const long aNewWidthOkay((aBtnOk.GetSizePixel().Width() * aNewSizeDialog.Width()) / aCurrentSizeDialog.Width());
        const long aNewPosOkay((aBtnOk.GetPosPixel().X() * aNewSizeDialog.Width()) / aCurrentSizeDialog.Width());
        const Size aNewSizeOkay(aNewWidthOkay, aBtnOk.GetOutputSizePixel().Height());
        aBtnOk.SetOutputSizePixel(aNewSizeOkay);
        aBtnOk.SetPosSizePixel(Point(aNewPosOkay, aNewButtonY), aNewSizeOkay);

        // for Cancel-Button
        const long aNewWidthCancel((aBtnCancel.GetSizePixel().Width() * aNewSizeDialog.Width()) / aCurrentSizeDialog.Width());
        const long aNewPosCancel((aBtnCancel.GetPosPixel().X() * aNewSizeDialog.Width()) / aCurrentSizeDialog.Width());
        const Size aNewSizeCancel(aNewWidthCancel, aBtnCancel.GetOutputSizePixel().Height());
        aBtnCancel.SetOutputSizePixel(aNewSizeCancel);
        aBtnCancel.SetPosSizePixel(Point(aNewPosCancel, aNewButtonY), aNewSizeCancel);

        // for Help-Button
        const long aNewWidthHelp((aBtnHelp.GetSizePixel().Width() * aNewSizeDialog.Width()) / aCurrentSizeDialog.Width());
        const long aNewPosHelp((aBtnHelp.GetPosPixel().X() * aNewSizeDialog.Width()) / aCurrentSizeDialog.Width());
        const Size aNewSizeHelp(aNewWidthHelp, aBtnHelp.GetOutputSizePixel().Height());
        aBtnHelp.SetOutputSizePixel(aNewSizeHelp);
        aBtnHelp.SetPosSizePixel(Point(aNewPosHelp, aNewButtonY), aNewSizeHelp);

        // set new sizes for color control
        aTabBgColorSet.SetOutputSizePixel(aNewSize);
        aTabBgColorSet.SetPosSizePixel(Point(nAdd/2, nAdd/2), aNewSize);

        // set new size for border win
        aBorderWin.SetOutputSizePixel(aNewSizeBorderWin);

        // set new size for dialog itself
        SetOutputSizePixel(aNewSizeDialog);
    }

    aTabBgColorSet.SelectItem(nSelectedItem);
    aTabBgColorSet.Resize();
}

IMPL_LINK( ScTabBgColorDlg, TabBgColorDblClickHdl_Impl, ValueSet*, EMPTYARG )
/*
    Handler, called when color selection is changed
*/
{
    sal_uInt16 nItemId = aTabBgColorSet.GetSelectItemId();
    Color aColor = nItemId ? ( aTabBgColorSet.GetItemColor( nItemId ) ) : Color( COL_AUTO );
    aTabBgColor = aColor;
    EndDialog( sal_True );
    return 0;
}

IMPL_LINK( ScTabBgColorDlg, TabBgColorOKHdl_Impl, OKButton*, EMPTYARG )
{

//    Handler, called when the OK button is pushed

    sal_uInt16 nItemId = aTabBgColorSet.GetSelectItemId();
    Color aColor = nItemId ? ( aTabBgColorSet.GetItemColor( nItemId ) ) : Color( COL_AUTO );
    aTabBgColor = aColor;
    EndDialog( sal_True );
    return 0;
}

ScTabBgColorDlg::ScTabBgColorValueSet::ScTabBgColorValueSet( Control* pParent, const ResId& rResId, ScTabBgColorDlg* pTabBgColorDlg ) :
    SvxColorValueSet(pParent, rResId)
{
    aTabBgColorDlg = pTabBgColorDlg;
}

void ScTabBgColorDlg::ScTabBgColorValueSet::KeyInput( const KeyEvent& rKEvt )
{
    switch ( rKEvt.GetKeyCode().GetCode() )
    {
        case KEY_SPACE:
        case KEY_RETURN:
        {
            sal_uInt16 nItemId = GetSelectItemId();
            const Color& aColor = nItemId ? ( GetItemColor( nItemId ) ) : Color( COL_AUTO );
            aTabBgColorDlg->aTabBgColor = aColor;
            aTabBgColorDlg->EndDialog(sal_True);
        }
        break;
    }
    SvxColorValueSet::KeyInput(rKEvt);
}
