//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
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

#include <QApplication>
#include <memory>
#include "CombiDialog.h"
#include "BankDialog.h"
#include "InstrumDialog.h"
#include "model/Scene.h"
#include "model/MidiCombi.h"

class MyGLWindow;

class MyGLApplication : public QApplication {
    Q_OBJECT

public:
    MyGLApplication(int &argc, char *argv[]);
    ~MyGLApplication();
    MyGLWindow *GLwindow();
    void startIdleCallback(double fps);

    bool loadSceneFile();
    bool saveSceneFile();
    bool saveCloudFile(SceneCloud *selectedCloudSave);
    bool loadCloudDefaultFile();
    void addSample();
    void showDialogVolumeEnvelope(SceneCloud *selectedCloudToVolumeEnvelope);
    void showCloudDialog(SceneCloud *selectedCloud);
    void destroyCloudDialog(unsigned selectedCloudId);
    void midiNoteOn (int midiChannelToPlay, int midiKeyToPlay, int midiVeloToPlay);
    void midiNoteOff (int midiChannelToStop, int midiKeyToStop);
    void destroyAllCloudDialogs();
    void showOptionsDialog();
    void showMidiBankDialog();
    void showMidiInstrumentDialog();

signals:
    void oscMessageArrived(const QByteArray &message);

private:
    struct Impl;
    std::unique_ptr<Impl> P;
    Combination *myCombi;
    CombiDialog *combiDialog;
    BankDialog *bankDialog;
    InstrumentDialog *instrumentDialog;
};

#define theApplication \
    (static_cast<MyGLApplication *>(QApplication::instance()))
