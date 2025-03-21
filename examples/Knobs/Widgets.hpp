/*
 * AIDA-X Widgets
 * Copyright (C) 2023 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "EventHandlers.hpp"
#include "ImageWidgets.hpp"
#include "NanoVG.hpp"

#include "DistrhoPluginInfo.h"

START_NAMESPACE_DISTRHO

static constexpr const uint kSubWidgetsFontSize = 14;
static constexpr const uint kSubWidgetsFullHeight = 90;
static constexpr const uint kSubWidgetsPadding = 8;

static inline constexpr
float normalizedLevelMeterValue(const float db)
{
	return (
        db < -70.f ? 0.f :
        db < -60.f ? (db + 70.f) * 0.25f :
        db < -50.f ? (db + 60.f) * 0.50f +  2.5f :
        db < -40.f ? (db + 50.f) * 0.75f +  7.5f :
        db < -30.f ? (db + 40.f) * 1.50f + 15.0f :
        db < -20.f ? (db + 30.f) * 2.00f + 30.0f :
        db <   0.f ? (db + 20.f) * 2.50f + 50.0f :
        100.f
    ) / 100.f;
}

// --------------------------------------------------------------------------------------------------------------------
// good old knob with tick markers, rotating image and on-actived value display, follows modgui style

class AidaKnob : public NanoSubWidget,
                 public KnobEventHandler
{
    NanoTopLevelWidget* const parent;
    const NanoImage& knobImage;
    const NanoImage& scaleImage;
    
    public:
    static constexpr const uint kScaleSize = 80;
    static constexpr const uint kKnobSize = 55;
    static constexpr const uint kKnobMargin = (kScaleSize - kKnobSize) / 2;

    const char* label;
    const char* unit;

    AidaKnob(NanoTopLevelWidget* const p, KnobEventHandler::Callback* const cb,
             const NanoImage& knobImg, const NanoImage& scaleImg)
        : NanoSubWidget(p),
          KnobEventHandler(this),
          parent(p),
          knobImage(knobImg),
          scaleImage(scaleImg)
    {
        const double scaleFactor = p->getScaleFactor();
        setSize(kScaleSize * scaleFactor, kSubWidgetsFullHeight * scaleFactor);
        setCallback(cb);

    }

protected:
    void onNanoDisplay() override
    {
        const uint width = getWidth();
        const uint height = getHeight();

        const double scaleFactor = parent->getScaleFactor();
        const double scaleSize = kScaleSize * scaleFactor;
        const double knobSize = kKnobSize * scaleFactor;
        const double knobHalfSize = knobSize / 2;
        const double knobMargin = kKnobMargin * scaleFactor;
        const double wfontSize = kSubWidgetsFontSize * scaleFactor;

        beginPath();
        rect(0, 0, scaleSize, scaleSize);
        fillPaint(imagePattern(0, 0, scaleSize, scaleSize, 0.f, scaleImage, 1.f));
        fill();

        fillColor(Color(1.f, 1.f, 1.f));
        fontSize(wfontSize);
        textAlign(ALIGN_CENTER | ALIGN_BASELINE);
        text(width/2, height, label, nullptr);

        const Paint knobImgPat = imagePattern(-knobHalfSize, -knobHalfSize, knobSize, knobSize, 0.f, knobImage, 1.f);

        save();
        translate(knobMargin + knobHalfSize, knobMargin + knobHalfSize);
        rotate(degToRad(270.f * (getNormalizedValue() - 0.5f)));

        beginPath();
        rect(-knobHalfSize, -knobHalfSize, knobSize, knobSize);
        fillPaint(knobImgPat);
        fill();

        restore();

        if (getState() & kKnobStateDragging)
        {
            const double padding = 4 * scaleFactor;
            beginPath();
            roundedRect(padding, 0,
                        scaleSize - padding,
                        wfontSize + padding * 2,
                        2 * scaleFactor);
            fillColor(Color(0,0,0,0.5f));
            fill();

            char textBuf[24];
            std::snprintf(textBuf, sizeof(textBuf)-1, "%.2f %s", getValue(), unit);
            textBuf[sizeof(textBuf)-1] = '\0';

            fillColor(Color(1.f, 1.f, 1.f));
            textAlign(ALIGN_CENTER | ALIGN_TOP);
            text(width/2, padding, textBuf, nullptr);
        }
    }

    bool onMouse(const MouseEvent& event) override
    {
        return KnobEventHandler::mouseEvent(event);
    }

    bool onMotion(const MotionEvent& event) override
    {
        return KnobEventHandler::motionEvent(event);
    }

    bool onScroll(const ScrollEvent& event) override
    {
        return KnobEventHandler::scrollEvent(event);
    }
};

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
