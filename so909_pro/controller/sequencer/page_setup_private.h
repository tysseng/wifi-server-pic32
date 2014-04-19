#ifndef _PAGE_SETUP_PRIVATE_H
#define _PAGE_SETUP_PRIVATE_H

#include "../shared/types.h"

void PGS_inputActionSetupVelocity(struct InputField *field, short enter);
void PGS_inputActionSetupAccent(struct InputField *field, short enter);
void PGS_inputActionSetupBasenote(struct InputField *field, short enter);
void PGS_inputActionSetupMidiChannel(struct InputField *field, short enter);

void PGS_inputActionSetupSteps(struct InputField *field, short enter);
void PGS_inputActionSetupRes(struct InputField *field, short enter);
void PGS_inputActionSetupBpb(struct InputField *field, short enter);
void PGS_inputActionSetupTracks(struct InputField *field, short enter);
void PGS_inputActionSetupSwing(struct InputField *field, short enter);
void PGS_inputActionSetupSetBpm(struct InputField *field, short enter);

void PGS_inputActionSetupExternalSynch(struct InputField *field, short enter);
void PGS_inputActionSetupSysexAddr1(struct InputField *field, short enter);
void PGS_inputActionSetupSysexAddr2(struct InputField *field, short enter);
void PGS_inputActionSetupSysexDump(struct InputField *field, short enter);
void PGS_inputActionSetupResetSteps(struct InputField *field, short enter);
void PGS_inputActionSetupResetSong(struct InputField *field, short enter);
void PGS_inputActionSetupResetPerformance(struct InputField *field, short enter);
void PGS_inputActionSetupResetSystem(struct InputField *field, short enter);

void PGS_initInputFields();

void PGS_updateSetupTrack();
void PGS_updateSetupPattern();
void PGS_updateSetupGeneral();

void PGS_toggleSetupParam(unsigned short param);
void PGS_toggleSetupItem(unsigned short newItem);
#endif