//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     https://github.com/linuxmao-org/Frontieres
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2018  Jean Pierre Cimalando
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "interface/StartDialog.h"
#include "ui_StartDialog.h"

struct StartDialog::Impl {
    Ui::StartDialog ui;
    int choice = -1;
};

StartDialog::StartDialog()
    : P(new Impl)
{
    P->ui.setupUi(this);

    connect(P->ui.btnNewScene, &QPushButton::clicked,
        this, [this] { P->choice = Choice_NewScene; accept(); });
    connect(P->ui.btnLoadScene, &QPushButton::clicked,
        this, [this] { P->choice = Choice_LoadScene; accept(); });
}

StartDialog::~StartDialog()
{
}

int StartDialog::choiceResult()
{
    return P->choice;
}
