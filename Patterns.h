#pragma once

PBYTE INTERSECT_SEGMENT_PATTERN_X64 = (PBYTE)"\x48\x89\x54\x24\x00\x48\x89\x4C\x24\x00\x48\x83\xEC\x28\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x00\x4C\x8B\x44\x24\x00\x48\x8B\x54\x24\x00\x48\x8B\x0D\x00\x00\x00\x00\xFF\x50\x18\x48\x83\xC4\x28\xC3";
const char* INTERSECT_SEGMENT_MASK_X64 = "xxxx?xxxx?xxxxxxx????xxxxxxx?xxxx?xxx????xxxxxxxx";
PBYTE GET_NODE_TRANSFORM_PATTERN_X64 = (PBYTE)"\x48\x8B\x44\x24\x00\x48\x89\x44\x24\x00\x44\x0F\xB6\x4C\x24\x00\x4C\x8B\x44\x24\x00\x8B\x54\x24\x50\x48\x8B\x4C\x24\x00\xE8\x00\x00\x00\x00";
const char* GET_NODE_TRANSFORM_MASK_X64 = "xxxx?xxxx?xxxxx?xxxx?xxxxxxxx?x????";
PBYTE SET_OBJECT_DIMS_PATTERN_X64 = (PBYTE)"\x44\x89\x4C\x24\x00\x4C\x89\x44\x24\x00\x48\x89\x54\x24\x00\x48\x89\x4C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\x4C\x24\x00\xE8\x00\x00\x00\x00\x48\x83\xBC\x24\x00\x00\x00\x00\x00\x74\x0B\x48\x83\xBC\x24\x00\x00\x00\x00\x00\x75\x3E\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\x3D\x00\x00\x00\x00\x00\x7C\x21";
const char* SET_OBJECT_DIMS_MASK_X64 = "xxxx?xxxx?xxxx?xxxx?xxx????xxxx?x????xxxx?????xxxxxx?????xxx????x????xx?????xx";