#pragma once

const DWORD64 _endScene = 42;
const DWORD64 _drawIndexedPrimitive = 82;
const DWORD64 _reset = 16;
const DWORD64 _present = 17;

auto defaultGoldHead = LTVector(8., 8., 8.);
auto defaultSilverHead = LTVector(17., 17., 17.);
auto defaultBody = LTVector(20., 20., 20.);
auto defaultPelvis = LTVector(23., 23., 23.);

auto rightHandBones = std::vector<int>{ 15, 16, 17 };
auto leftHandBones = std::vector<int>{ 8, 9, 10 };
auto rightLegBones = std::vector<int>{ 25, 26, 27 };
auto leftLegBones = std::vector<int>{ 21, 22, 23 };
auto upperBodyBones = std::vector<int>{ 5, 14 };
auto lowerBodyBones = std::vector<int>{ 3, 4 };

auto rightHandFirstBone = 16;
auto leftHandFirstBone = 9;
auto leftLegFirstBone = 23;
auto rightLegFirstBone = 27;

auto mainBones = std::vector<int>{ 5, 4, 3, 2, 1, 0, 26, 22};

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'