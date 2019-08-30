//First Cell Where You Meet The oldman

#include "cell1scene.h"

typedef twaf::Scene* Scene_Ptr;

extern "C" Scene_Ptr __declspec(dllimport) __stdcall CreateScene(twaf::Graphics& graphics);

Scene_Ptr __stdcall CreateScene(twaf::Graphics& graphics)
{
	return new twaf::Cell1Scene(graphics);
}