#include "ML_modules.hpp"

#include "dsp/digital.hpp"

struct OctaSwitch : Module {
	enum ParamIds {
		THRESHOLD_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		THRESHOLD_INPUT,
		GATE_INPUT,
		A_INPUT    = GATE_INPUT + 8,
		B_INPUT    = A_INPUT + 8,
		NUM_INPUTS = B_INPUT + 8
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS = OUT_OUTPUT + 8
	};

	enum LightIds {
		NUM_LIGHTS
	};

	float threshold = 0.0;


	OctaSwitch() : Module( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS ) { };


	void step() override;

};



void OctaSwitch::step() {

	bool gate[8];

	threshold = inputs[THRESHOLD_INPUT].normalize(params[THRESHOLD_PARAM].value);

	gate[0] = inputs[GATE_INPUT].normalize(0.0) > threshold;


	for(int i=1; i<8; i++) {
		
		if( inputs[GATE_INPUT+i].active ) gate[i] = inputs[GATE_INPUT+i].value > threshold;
		else	                          gate[i] = gate[i-1];

	}



	for(int i=0; i<8; i++) { 
		outputs[OUT_OUTPUT+i].value  = gate[i] ? inputs[B_INPUT+i].normalize(0.0) : inputs[A_INPUT+i].normalize(0.0);
	}

};

struct ThresholdDisplayWidget : TransparentWidget {

  float  *value;

  std::shared_ptr<Font> font;

  ThresholdDisplayWidget() {
    font = Font::load(assetPlugin(plugin, "res/Segment7Standard.ttf"));
  };

  void draw(NVGcontext *vg) {
    // Background
    NVGcolor backgroundColor = nvgRGB(0x44, 0x44, 0x44);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(vg, backgroundColor);
    nvgFill(vg);
    nvgStrokeWidth(vg, 1.0);
    nvgStrokeColor(vg, borderColor);
    nvgStroke(vg);

    nvgFontSize(vg, 18);
    nvgFontFaceId(vg, font->handle);
    nvgTextLetterSpacing(vg, 1.0);

    char display_string[10];

    sprintf(display_string,"%5.1f",*value);

    Vec textPos = Vec(3.0f, 17.0f);

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "~~~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "\\\\\\\\", NULL);

	{
	    textColor = nvgRGB(0xf0, 0x00, 0x00);
		nvgFillColor(vg, textColor);
		nvgText(vg, textPos.x, textPos.y, display_string, NULL);
	};
  }
};

OctaSwitchWidget::OctaSwitchWidget() {

	OctaSwitch *module = new OctaSwitch();
	setModule(module);
	box.size = Vec(15*10, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/OctaSwitch.svg")));

		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 365)));



	const float offset_y = 62, delta_y = 32, row1=15, row2 = 47, row3 = 77, row4 = 110;

	addInput(createInput<PJ301MPort>(   Vec(row1,  328 ), module, OctaSwitch::THRESHOLD_INPUT));
	addParam(createParam<SmallMLKnob>(  Vec(row2-5,  326), module, OctaSwitch::THRESHOLD_PARAM, -5.0, 10.0, 1.0));


	for( int i=0; i<8; i++) {
		addInput(createInput<PJ301MPort>(Vec(row1, offset_y + i*delta_y ), module, OctaSwitch::GATE_INPUT+i));
		addInput(createInput<PJ301MPort>(Vec(row2, offset_y + i*delta_y ), module, OctaSwitch::A_INPUT+i));
		addInput(createInput<PJ301MPort>(Vec(row3, offset_y + i*delta_y ), module, OctaSwitch::B_INPUT+i));

		addOutput(createOutput<PJ301MPort>(Vec(row4, offset_y + i*delta_y ), module, OctaSwitch::OUT_OUTPUT+i));
	};

	ThresholdDisplayWidget *display = new ThresholdDisplayWidget();
	display->box.pos = Vec(row3-3,330);
	display->box.size = Vec(65, 20);
	display->value = &module->threshold;
	addChild(display);

}

