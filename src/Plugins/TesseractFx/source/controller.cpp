// -----------------------------------------------------------------------------------------------------------------------------
// This file is part of the �berton project. Copyright (C) 2021 �berton
//
// �berton is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// �berton is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
// have received a copy of the GNU General Public License along with �berton. If not, see http://www.gnu.org/licenses/.
// -----------------------------------------------------------------------------------------------------------------------------


#include "controller.h"
#include "ids.h"
#include <vstmath.h>

namespace Uberton {
namespace TesseractFx {

tresult PLUGIN_API Controller::initialize(FUnknown* context) {

	tresult result = ControllerBase::initialize(context);
	if (result != kResultTrue) return result;



	// Set up units
	UnitID rootUnitId = 1;
	addUnit(new Unit(USTRING("Root"), rootUnitId));
	setCurrentUnitID(rootUnitId);
	UnitID inputPositionUnitId = 2;
	addUnit(new Unit(USTRING("Input Position"), inputPositionUnitId, rootUnitId));
	UnitID outputPositionUnitId = 3;
	addUnit(new Unit(USTRING("Output Position"), outputPositionUnitId, rootUnitId));
	UnitID postSectionUnitId = 4;
	addUnit(new Unit(USTRING("Post Filter"), postSectionUnitId, rootUnitId));


	setCurrentUnitID(rootUnitId);
	{
		//addParam<LinearParameter>(ParamSpecs::vol, L"Master Volume", L"MVol", L"", Precision(2));
		addParam<LogParameter>(ParamSpecs::vol, L"Master Volume", L"MVol", L"", Precision(2));
		//parameters.addParameter(new GainParameter(L"Master Volume", ParamSpecs::vol.id, L"dB"));
		addParam<LinearParameter>(ParamSpecs::mix, L"Mix", L"Mix", L"%", Precision(0));
		addParam<DiscreteParameter>(ParamSpecs::resonatorType, "Resonator Type", "Res Type", "")->getInfo().stepCount = 1;
		addParam<DiscreteParameter>(ParamSpecs::resonatorDim, "Resonator Dimension", "Res Dim", "D", Precision(0))->getInfo().stepCount = maxDimension - 1;
		addParam<DiscreteParameter>(ParamSpecs::resonatorOrder, "Resonator Order", "Res Order", "")->getInfo().stepCount = maxOrder - 1;

		addParam<LogParameter>(ParamSpecs::damp, L"Resonator Dampening", L"Res Damp", L"", Precision(2));
		addParam<LogParameter>(ParamSpecs::freq, L"Resonator Frequency", L"Res Freq", L"Hz");

		addParam<LinearParameter>(ParamSpecs::resonatorVel, "Sonic Velocity", "Sonic Vel", "m/s");

		addParam<LinearParameter>(ParamSpecs::inPosCurveL, "Input Pos Curve L", "InPos L", L"", Precision(3));
		addParam<LinearParameter>(ParamSpecs::inPosCurveR, "Input Pos Curve R", "InPos R", L"", Precision(3));
		addParam<LinearParameter>(ParamSpecs::outPosCurveL, "Output Pos Curve L", "OutPos L", L"", Precision(3));
		addParam<LinearParameter>(ParamSpecs::outPosCurveR, "Output Pos Curve R", "OutPos R", L"", Precision(3));
		addStringListParam(ParamSpecs::linkInPosCurves, "Link In Pos Curves", "Link In C", { "Not Linked", "Linked" });
		addStringListParam(ParamSpecs::linkOutPosCurves, "Link Out Pos Curves", "Link Out C", { "Not Linked", "Linked" });

		//addParam<LinearParameter>(ParamSpecs::vuPPM, "Output Level", "Level", "dB", ParameterInfo::kIsReadOnly);
		parameters.addParameter(new GainParameter(L"Output Level", ParamSpecs::vuPPM.id, L"dB",0,ParameterInfo::kIsReadOnly,rootUnitId,L"Level"));
	}

	setCurrentUnitID(postSectionUnitId);
	{
		addParam<LogParameter>(ParamSpecs::lcFreq, L"Low Cut Frequency", L"LC Freq");
		addParam<LinearParameter>(ParamSpecs::lcQ, "Low Cut Q", "LC Q", "");
		addParam<LogParameter>(ParamSpecs::hcFreq, L"High Cut Frequency", L"HC Freq");
		addParam<LinearParameter>(ParamSpecs::hcQ, "High Cut Q", "HC Q", "");
	}

	setCurrentUnitID(inputPositionUnitId);
	{
		UString256 a("", 10);
		for (int i = 0; i < maxDimension; i++) {
			a.printInt(i);
			UString256 name("Left X");
			name.append(a);
			addRangeParam(Params::kParamInL0 + i, name, "", { 0, 1, .5 });
		}
		for (int i = 0; i < maxDimension; i++) {
			a.printInt(i);
			UString256 name("Right X");
			name.append(a);
			addRangeParam(Params::kParamInR0 + i, name, "", { 0, 1, .5 });
		}
		setCurrentUnitID(outputPositionUnitId);
		for (int i = 0; i < maxDimension; i++) {
			a.printInt(i);
			UString256 name("Left Y");
			name.append(a);
			addRangeParam(Params::kParamOutL0 + i, name, "", { 0, 1, .5 });
		}
		for (int i = 0; i < maxDimension; i++) {
			a.printInt(i);
			UString256 name("Right Y");
			name.append(a);
			addRangeParam(Params::kParamOutR0 + i, name, "", { 0, 1, .5 });
		}
	}

	return kResultTrue;
}

IPlugView* PLUGIN_API Controller::createView(FIDString name) {
	if (ConstString(name) == ViewType::kEditor) return new VSTGUI::VST3Editor(this, "Editor", "editor.uidesc");
	return nullptr;
}

} // namespace TesseractFx
} // namespace Uberton