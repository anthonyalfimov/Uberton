
// Controller classes with extended features such as exporting a bypass parameter by default
// or setting the component state through a ParamStateClass (see below).
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

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <vstgui/plugin-bindings/vst3editor.h>
#include <pluginterfaces/base/ustring.h>
#include <array>
#include "parameters.h"
#include "ActionHistory.h"


namespace Uberton {
using namespace Steinberg;
using namespace Steinberg::Vst;

class HistorySubcontroller;

class HistoryControllerBase : public EditControllerEx1, public VSTGUI::VST3EditorDelegate
{
	using Parent = EditControllerEx1;

public:
	VSTGUI::IController* PLUGIN_API createSubController(VSTGUI::UTF8StringPtr name, const VSTGUI::IUIDescription* description, VSTGUI::VST3Editor* editor) SMTG_OVERRIDE;

	void PLUGIN_API editorAttached(EditorView* editor) SMTG_OVERRIDE;
	void PLUGIN_API editorRemoved(EditorView* editor) SMTG_OVERRIDE;

	tresult PLUGIN_API getState(IBStream* stream) SMTG_OVERRIDE;
	tresult PLUGIN_API setState(IBStream* stream) SMTG_OVERRIDE;

	tresult beginEdit(Vst::ParamID id) SMTG_OVERRIDE;
	tresult endEdit(Vst::ParamID id) SMTG_OVERRIDE;

	void undo();
	void redo();
	void applyAction(ParamID id, ParamValue value);
	std::wstring actionToString(const Action& action);

	void updateHistoryButtons();
	void updateCurrentZoomFactor();
	ActionHistory history;
	ParamValue startValue{ 0 };

	static const ParamID invalidParamID = std::numeric_limits<ParamID>::max();
	ParamID currentlyEditedParam{ invalidParamID };

	using TheEditor = VSTGUI::VST3Editor;
	std::vector<TheEditor*> editors;
	std::map<EditorView*, HistorySubcontroller*> hcm;

	double currentZoomFactor{ 1 };
	int64 stateVersion{ 0 };
};

/*
 * Controller base class that uses some kind of global parameter state with specifications
 * as described in parameters.h. The bypass parameter is exported automatically if enabled.
 * The component state is set through the member function of the ParamStateClass class.
 */
template<class ParamStateClass, bool hasBypass>
class ControllerBase : public HistoryControllerBase
{
public:
	ControllerBase() {}

	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE {
		tresult result = EditControllerEx1::initialize(context);
		if (result != kResultTrue) return result;

		if (hasBypass)
			parameters.addParameter(Steinberg::UString256("Bypass"), nullptr, 1, 0, ParameterInfo::kIsBypass | ParameterInfo::kCanAutomate, bypassId);

		return kResultTrue;
	}

	// Called when a preset is loaded
	tresult PLUGIN_API setComponentState(IBStream* state) SMTG_OVERRIDE {
		ParamStateClass paramState;
		tresult result = paramState.setComponentState(state, *this);
		if (hasBypass)
			setParamNormalized(bypassId, paramState.isBypassed());
		return result;
	}

	RangeParameter* addRangeParam(Vst::ParamID id, UString256 name, UString256 units, std::array<ParamValue, 3> minMaxDefault, bool readonly = false) {
		int flags = 0;
		if (readonly) {
			flags |= ParameterInfo::kIsReadOnly;
		} else {
			flags |= ParameterInfo::kCanAutomate;
		}
		auto p = new RangeParameter(name, id, units, minMaxDefault[0], minMaxDefault[1], minMaxDefault[2], 0, flags, currentUnitID);
		parameters.addParameter(p);
		return p;
	}
	using Precision = int32;

	RangeParameter* addRangeParam(const ParamSpec& paramDesc, const UString256& title, const UString256& shortTitle, const UString256& units = "", Precision precision = 1, int32 flags = ParameterInfo::kCanAutomate) {
		auto p = new RangeParameter(title, paramDesc.id, units, paramDesc.minValue, paramDesc.maxValue, paramDesc.defaultValue, 0, flags, currentUnitID, shortTitle);
		parameters.addParameter(p);
		p->setPrecision(precision);
		return p;
	}

	/*RangeParameter* addParam(const LinearParamSpec& paramSpec, const UString256& title, const UString256& shortTitle, const UString256& units = "", int precision = 1, int32 flags = ParameterInfo::kCanAutomate) {
		auto p = parameters.addParameter(new LinearParameter(paramSpec, title, shortTitle, units, flags, currentUnitID));
		p->setPrecision(precision);
	}*/
	template<class ScaledParameter>
	Parameter* addParam(const typename ScaledParameter::ParamSpecType& paramSpec, const UString256& title, const UString256& shortTitle, const UString256& units = "", Precision precision = 1, int32 flags = ParameterInfo::kCanAutomate) {
		auto p = parameters.addParameter(new ScaledParameter(paramSpec, title, shortTitle, units, flags, currentUnitID));
		p->setPrecision(precision);
		return p;
	}

	// Explicit template specialization for discrete parameters (precision: 0, stepCount: automatic)
	template<>
	Parameter* addParam<DiscreteParameter>(const typename DiscreteParameter::ParamSpecType& paramSpec, const UString256& title, const UString256& shortTitle, const UString256& units, Precision precision, int32 flags) {
		auto p = parameters.addParameter(new DiscreteParameter(paramSpec, title, shortTitle, units, flags, currentUnitID));
		p->setPrecision(0);
		p->getInfo().stepCount = paramSpec.scale.getMax() - paramSpec.scale.getMin() - 1;
		return p;
	}


	StringListParameter* addStringListParam(const ParamSpec& paramDesc, const UString256& title, const UString256& shortTitle, const std::initializer_list<UString256>& strings, int32 flags = ParameterInfo::kCanAutomate) {
		auto p = new StringListParameter(title, paramDesc.id, nullptr, flags, currentUnitID, shortTitle);
		parameters.addParameter(p);
		for (const auto& s : strings) {
			p->appendString(s);
		}
		return p;
	}

	// all parameters created with addRangeParam() after setting current unit id will get this id
	void setCurrentUnitID(UnitID id) {
		currentUnitID = id;
	}

	UnitID currentUnitID = kRootUnitId;
	const bool ReadOnly = true;
};

/*
 * - can automatically add a bypass parameter(set implementBypass to false in constructor to avoid doing so)
 */
class ControllerBase1 : public EditControllerEx1
{
public:
	ControllerBase1();

	tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;

	bool implementBypass = true;
};

}