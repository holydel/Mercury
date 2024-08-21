#define MINIAUDIO_IMPLEMENTATION
#include "ma_llsi.h"
#include "mercury_log.h"
#include "miniaudio.h"

ma_engine gEngine;
using namespace mercury;

bool llsi::initialize()
{
	write_log_message("LowLevel Sound Interface Initialize: MiniAudio");

	auto result = ma_engine_init(NULL, &gEngine);

	ma_engine_play_sound(&gEngine, "C:\\Users\\holyd\\AppData\\Roaming\\odh\\packages\\other-packages\\oculus-ambisonics-starter-pack\\Oculus Ambisonic Audio Pack 01\\Nature and Outdoors\\Afternoon Suburban Park Babbling Brook Birds ST450 02_ambiX.wav", NULL);
	ma_engine_set_volume(&gEngine, 0.1f);

	return true;
}

void llsi::shutdown()
{
	ma_engine_uninit(&gEngine);

	write_log_message("LowLevel Sound Interface Shutdown: MiniAudio");
}

bool llsi::update()
{
	return true;
}
