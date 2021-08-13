
// Plugin ids (for processor, controller and parameters) for TesseractFx
//
// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the �berton project. Copyright (C) 2021 �berton
//
// �berton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// �berton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with �berton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#pragma once
#include <pluginterfaces/vst/vsttypes.h>
#include <parameters.h>

namespace Uberton {
namespace TesseractFx {

constexpr uint64 stateVersion = 0;

constexpr int maxDimension = 10;
constexpr int maxOrder = 200;


enum Params : Steinberg::Vst::ParamID {
	kParamVol,
	kParamMix,

	kParamResonatorType,
	kParamResonatorDim,
	kParamResonatorOrder,
	kParamResonatorFreq,
	kParamResonatorDamp,
	kParamResonatorVel,

	kParamLCFreq,
	kParamLCQ,
	kParamHCFreq,
	kParamHCQ,

	kParamInL0,
	kParamInLN = kParamInL0 + maxDimension - 1,
	kParamInR0,
	kParamInRN = kParamInR0 + maxDimension - 1,
	kParamOutL0,
	kParamOutLN = kParamOutL0 + maxDimension - 1,
	kParamOutR0,
	kParamOutRN = kParamOutR0 + maxDimension - 1,

	kParamInPosCurveL,
	kParamInPosCurveR,
	kParamOutPosCurveL,
	kParamOutPosCurveR,
	kParamLinkInPosCurves,
	kParamLinkOutPosCurves,

	kParamVUPPM, // OUT
	kParamProcessTime, // OUT
	kNumGlobalParameters
};

namespace ParamSpecs {

// id, min, max, default, initial (all scaled not normalized!)
static const LogParamSpec vol{ kParamVol, 0.0, 1.0, 0.8, 0.8 };
static const LinearParamSpec mix{ kParamMix, 0.0, 100, 100, 100 };

static const DiscreteParamSpec resonatorType{ kParamResonatorType, 1, 2, 1, 1 };
static const DiscreteParamSpec resonatorDim{ kParamResonatorDim, 1, maxDimension, 4, 3 };
static const DiscreteParamSpec resonatorOrder{ kParamResonatorOrder, 1, maxOrder, 5, 128 };

static const LogParamSpec freq{ kParamResonatorFreq, 20, 15000, 500, 200 };
static const LogParamSpec damp{ kParamResonatorDamp, 0, 10, 1, 2.3 };
static const LinearParamSpec resonatorVel{ kParamResonatorVel, 0.1, 1000.0, 10.0, 10.0 };

static const LogParamSpec lcFreq{ kParamLCFreq, 20, 5000, 200, 650 };
static const LinearParamSpec lcQ{ kParamLCQ, 1, 8, 1, 1 };
static const LogParamSpec hcFreq{ kParamHCFreq, 100, 18000, 18000, 18000 };
static const LinearParamSpec hcQ{ kParamHCQ, 1, 8, 1, 1 };

static const LinearParamSpec inPosCurveL{ kParamInPosCurveL, -1.0, 1.0, 0.0, 0.0 };
static const LinearParamSpec inPosCurveR{ kParamInPosCurveR, -1.0, 1.0, 0.0, 0.0 };
static const LinearParamSpec outPosCurveL{ kParamOutPosCurveL, -1.0, 1.0, 0.0, 0.0 };
static const LinearParamSpec outPosCurveR{ kParamOutPosCurveR, -1.0, 1.0, 0.0, 0.0 };
static const ParamSpec linkInPosCurves{ kParamLinkInPosCurves, 0, 1, 0, 0 };
static const ParamSpec linkOutPosCurves{ kParamLinkOutPosCurves, 0, 1, 0, 0 };

static const LinearParamSpec vuPPM{ kParamVUPPM, 0.0, 1.0, 0.0, 0.0 };
static const LinearParamSpec processTime{ kParamProcessTime, 0,10, 0.0, 0.0 };

}

using ParamState = UniformParamState<kNumGlobalParameters>;

static const Steinberg::FUID ProcessorUID(0x81b3f1ba, 0x02024999, 0x837809e4, 0xa0be69d0);
static const Steinberg::FUID ControllerUID(0x10f2c668, 0x566d47e6, 0xbd8950d1, 0x9aed5c60);

}
}