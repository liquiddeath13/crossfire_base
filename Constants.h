#pragma once
#include "Colors.h"

const DWORD64 _endScene = 42;
const DWORD64 _drawIndexedPrimitive = 82;
const DWORD64 _reset = 16;
const DWORD64 _present = 17;

auto defaultGoldHead = LTVector<float>(8., 8., 8.);
auto defaultSilverHead = LTVector<float>(17., 17., 17.);
auto defaultBody = LTVector<float>(20., 20., 20.);
auto defaultPelvis = LTVector<float>(23., 23., 23.);

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'