// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FFRankHunterEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


private:

	void AddTopLevelMenu(FMenuBarBuilder& MenuBarBuilder);
	void AddMenuEntry(FMenuBuilder& MenuBuilder);
	void DeleteEditorSaveGameFile();
	void DeleteAllFilesInSaveDirectory();
};