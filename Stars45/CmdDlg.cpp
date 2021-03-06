/*  Starshatter OpenSource Distribution
    Copyright (c) 1997-2004, Destroyer Studios LLC.
    All Rights Reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name "Destroyer Studios" nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    SUBSYSTEM:    Stars.exe
    FILE:         CmdDlg.cpp
    AUTHOR:       John DiCamillo


    OVERVIEW
    ========
    Operational Command Dialog Active Window class
*/

#include "MemDebug.h"
#include "CmdDlg.h"
#include "CmpFileDlg.h"
#include "CmpnScreen.h"
#include "Starshatter.h"
#include "Campaign.h"
#include "Combatant.h"
#include "CombatGroup.h"
#include "CombatUnit.h"
#include "ShipDesign.h"

#include "Game.h"
#include "DataLoader.h"
#include "Button.h"
#include "Video.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "ParseUtil.h"
#include "FormatUtil.h"

// +--------------------------------------------------------------------+

CmdDlg::CmdDlg(CmpnScreen* mgr)
    : cmpn_screen(mgr),
      txt_group(0), txt_score(0), txt_name(0), txt_time(0),
      btn_save(0), btn_exit(0), stars(0), campaign(0), mode(0)
{
    stars    = Starshatter::GetInstance();
    campaign = Campaign::GetCampaign();

    for (int i = 0; i < 5; i++)
    btn_mode[i] = 0;
}

CmdDlg::~CmdDlg()
{
}

// +--------------------------------------------------------------------+

void
CmdDlg::RegisterCmdControls(FormWindow* win)
{
    btn_save       = (Button*) win->FindControl(  1);
    btn_exit       = (Button*) win->FindControl(  2);

    for (int i = 0; i < 5; i++) {
        btn_mode[i] = (Button*) win->FindControl(100 + i);
    }

    txt_group = win->FindControl(200);
    txt_score = win->FindControl(201);
    txt_name  = win->FindControl(300);
    txt_time  = win->FindControl(301);
}

// +--------------------------------------------------------------------+

void
CmdDlg::ShowCmdDlg()
{
    campaign = Campaign::GetCampaign();

    if (txt_name) {
        if (campaign)
        txt_name->SetText(campaign->Name());
        else
        txt_name->SetText("No Campaign Selected");
    }

    ShowMode();

    if (btn_save)
    btn_save->SetEnabled(!campaign->IsTraining());

    if (btn_mode[MODE_FORCES]) btn_mode[MODE_FORCES]->SetEnabled(!campaign->IsTraining());
    if (btn_mode[MODE_INTEL])  btn_mode[MODE_INTEL]->SetEnabled(!campaign->IsTraining());
}

// +--------------------------------------------------------------------+

void
CmdDlg::ExecFrame()
{
    CombatGroup* g = campaign->GetPlayerGroup();
    if (g && txt_group)
    txt_group->SetText(g->GetDescription());

    if (txt_score) {
        char score[32];
        sprintf_s(score, "Team Score: %d", campaign->GetPlayerTeamScore()); txt_score->SetText(score); txt_score->SetTextAlign(DT_RIGHT);
    }

    if (txt_time) {
        double t = campaign->GetTime();
        char daytime[32];
        FormatDayTime(daytime, t);
        txt_time->SetText(daytime);
    }

    int unread = campaign->CountNewEvents();

    if (btn_mode[MODE_INTEL]) {
        if (unread > 0) {
            char text[64];
            sprintf_s(text, "INTEL (%d)", unread); 
            btn_mode[MODE_INTEL]->SetText(text);
        }
        else {
            btn_mode[MODE_INTEL]->SetText("INTEL");
        }
    }
}

// +--------------------------------------------------------------------+

void
CmdDlg::ShowMode()
{
    for (int i = 0; i < 5; i++) {
        if (btn_mode[i]) btn_mode[i]->SetButtonState(0);
    }

    if (mode < 0 || mode > 4)
    mode = 0;

    if (btn_mode[mode]) btn_mode[mode]->SetButtonState(1);
}

void
CmdDlg::OnMode(AWEvent* event)
{
    for (int i = MODE_ORDERS; i < NUM_MODES; i++) {
        Button* btn = btn_mode[i];

        if (event->window == btn) {
            mode = i;
        }
    }

    switch (mode) {
    case MODE_ORDERS:    cmpn_screen->ShowCmdOrdersDlg();    break;
    case MODE_THEATER:   cmpn_screen->ShowCmdTheaterDlg();   break;
    case MODE_FORCES:    cmpn_screen->ShowCmdForceDlg();     break;
    case MODE_INTEL:     cmpn_screen->ShowCmdIntelDlg();     break;
    case MODE_MISSIONS:  cmpn_screen->ShowCmdMissionsDlg();  break;
    default:             cmpn_screen->ShowCmdOrdersDlg();    break;
    }
}

void
CmdDlg::OnSave(AWEvent* event)
{
    if (campaign && cmpn_screen) {
        CmpFileDlg* fdlg = cmpn_screen->GetCmpFileDlg();

        cmpn_screen->ShowCmpFileDlg();
    }
}

void
CmdDlg::OnExit(AWEvent* event)
{
    if (stars) {
        Mouse::Show(false);
        stars->SetGameMode(Starshatter::MENU_MODE);
    }
}

// +--------------------------------------------------------------------+
