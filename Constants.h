#pragma once
#include "Colors.h"

const DWORD64 _endScene = 42;
const DWORD64 _drawIndexedPrimitive = 82;
const DWORD64 _reset = 16;
const DWORD64 _present = 17;

auto defaultGoldHead = LTVector(8., 8., 8.);
auto defaultSilverHead = LTVector(17., 17., 17.);
auto defaultBody = LTVector(20., 20., 20.);
auto defaultPelvis = LTVector(23., 23., 23.);

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'