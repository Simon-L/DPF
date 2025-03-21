/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2021 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoUI.hpp"

#include "extra/String.hpp"

#include "DistrhoPluginInfo.h"
#include "NanoButton.hpp"

#include "Widgets.hpp"

START_NAMESPACE_DISTRHO

using DGL_NAMESPACE::Button;
using DGL_NAMESPACE::ButtonEventHandler;
using DGL_NAMESPACE::SubWidget;

// -----------------------------------------------------------------------------------------------------------

inline void setupButton(Button& btn, const int y)
{
    btn.setAbsolutePos(5, y);
    btn.setLabel("Open...");
    btn.setSize(100, 30);
}

class KnobsExampleUI : public UI,
                        public ButtonEventHandler::Callback,
                        public KnobEventHandler::Callback
{
public:
    static const uint kInitialWidth  = 300;
    static const uint kInitialHeight = 150;

    KnobsExampleUI()
        : UI(kInitialWidth, kInitialHeight),
          fScale(1.0f)
    {
        std::memset(fParameters, 0, sizeof(fParameters));

#ifdef DGL_NO_SHARED_RESOURCES
        createFontFromFile("sans", "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf");
#else
        loadSharedResources();
#endif

        knob_img = createImageFromFile("examples/Knobs/knob.png", IMAGE_GENERATE_MIPMAPS);
        scale_img = createImageFromFile("examples/Knobs/scale.png", IMAGE_GENERATE_MIPMAPS);

        knob1 = new AidaKnob(this, this, knob_img, scale_img);
        knob1->setAbsolutePos(15,15);
        knob1->setId(kParameterKnob1);
        knob1->setRange(0, 1);
        knob1->setDefault(0.5);
        knob1->setValue(0.5, false);
        knob1->label = "Knob1";
        knob1->unit = "";

        knob2 = new AidaKnob(this, this, knob_img, scale_img);
        knob2->setAbsolutePos(115,15);
        knob2->setId(kParameterKnob2);
        knob2->setRange(10, 25e3);
        knob2->setDefault(440);
        knob2->setValue(440, false);
        knob2->setUsingLogScale(true);
        knob2->label = "Knob2";
        knob2->unit = "";

        knob3 = new AidaKnob(this, this, knob_img, scale_img);
        knob3->setAbsolutePos(215,15);
        knob3->setId(kParameterKnob3);
        knob3->setRange(-128, 128);
        knob3->setDefault(0.0);
        knob3->setValue(0.0, false);
        knob3->label = "Knob3";
        knob3->unit = "";


        setGeometryConstraints(kInitialWidth, kInitialHeight, false);
    }

    NanoImage knob_img;
    NanoImage scale_img;
    AidaKnob* knob1;
    AidaKnob* knob2;
    AidaKnob* knob3;

protected:
   /* --------------------------------------------------------------------------------------------------------
    * DSP/Plugin Callbacks */

   /**
      A parameter has changed on the plugin side.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
        fParameters[index] = value;
        repaint();
    }

   /**
      A state has changed on the plugin side.@n
      This is called by the host to inform the UI about state changes.
    */
    void stateChanged(const char* key, const char* value) override
    {
    }

   /* --------------------------------------------------------------------------------------------------------
    * Widget Callbacks */

   /**
      The NanoVG drawing function.
    */
    void onNanoDisplay() override
    {
    }

    void onResize(const ResizeEvent& ev) override
    {
        fScale = static_cast<float>(ev.size.getHeight())/kInitialHeight;

        UI::onResize(ev);
    }

    void knobDragStarted(SubWidget* const widget) override
    {
        editParameter(widget->getId(), true);
    }

    void knobDragFinished(SubWidget* const widget) override
    {
        editParameter(widget->getId(), false);
    }

    void knobValueChanged(SubWidget* const widget, float value) override
    {
        setParameterValue(widget->getId(), value);
    }

    void knobDoubleClicked(SubWidget* const widget) override
    {
        // static_cast<AidaKnob*>(widget)->setValue(kParameters[widget->getId()].ranges.def, true);
    }

    void buttonClicked(SubWidget* const widget, int) override
    {
    }

    // -------------------------------------------------------------------------------------------------------

private:
    // Parameters
    float fParameters[kParameterCount];

    // UI stuff
    float fScale;

   /**
      Set our UI class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobsExampleUI)
};

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
    return new KnobsExampleUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
