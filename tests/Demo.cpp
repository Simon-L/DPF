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

#ifndef DGL_OPENGL
#error OpenGL build required for Demo
#endif

#include "tests.hpp"

// #define DPF_TEST_POINT_CPP
#include "dgl/src/pugl.cpp"
#include "dgl/src/Application.cpp"
#include "dgl/src/ApplicationPrivateData.cpp"
#include "dgl/src/Geometry.cpp"
#include "dgl/src/ImageBase.cpp"
#include "dgl/src/OpenGL.cpp"
#include "dgl/src/SubWidget.cpp"
#include "dgl/src/SubWidgetPrivateData.cpp"
#include "dgl/src/TopLevelWidget.cpp"
#include "dgl/src/TopLevelWidgetPrivateData.cpp"
#include "dgl/src/Widget.cpp"
#include "dgl/src/WidgetPrivateData.cpp"
#include "dgl/src/Window.cpp"
#include "dgl/src/WindowPrivateData.cpp"
#include "dgl/StandaloneWindow.hpp"

#include "widgets/ExampleColorWidget.hpp"
#include "widgets/ExampleImagesWidget.hpp"
#include "widgets/ExampleRectanglesWidget.hpp"
#include "widgets/ExampleShapesWidget.hpp"

#include "demo_res/DemoArtwork.cpp"
#include "images_res/CatPics.cpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------
// Left side tab-like widget

class LeftSideWidget : public SubWidget
{
public:
    static const int kPageCount = 5;

    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void curPageChanged(int curPage) = 0;
    };

    LeftSideWidget(Widget* parent, Callback* const cb)
        : SubWidget(parent),
          callback(cb),
          curPage(0),
          curHover(-1)
    {
#if 0
        // for text
        font = nvg.createFontFromFile("sans", "./nanovg_res/Roboto-Regular.ttf");
#endif

        using namespace DemoArtwork;
        img1.loadFromMemory(ico1Data, ico1Width, ico1Height, GL_BGR);
        img2.loadFromMemory(ico2Data, ico2Width, ico2Height, GL_BGR);
        img3.loadFromMemory(ico3Data, ico3Width, ico2Height, GL_BGR);
        img4.loadFromMemory(ico4Data, ico4Width, ico4Height, GL_BGR);
        img5.loadFromMemory(ico5Data, ico5Width, ico5Height, GL_BGR);
    }

protected:
    void onDisplay() override
    {
        const int iconSize = bgIcon.getWidth();

        glColor3f(0.027f, 0.027f, 0.027f);
        Rectangle<uint>(0, 0, getSize()).draw();

        bgIcon.setY(curPage*iconSize + curPage*3);

        glColor3f(0.129f, 0.129f, 0.129f);
        bgIcon.draw();

        glColor3f(0.184f, 0.184f, 0.184f);
        bgIcon.drawOutline();

        if (curHover != curPage && curHover != -1)
        {
            Rectangle<int> rHover(1, curHover*iconSize + curHover*3, iconSize-2, iconSize-2);

            glColor3f(0.071f, 0.071f, 0.071f);
            rHover.draw();

            glColor3f(0.102f, 0.102f, 0.102f);
            rHover.drawOutline();
        }

        glLineWidth(2.0f);
        glColor3f(0.184f, 0.184f, 0.184f);
        lineSep.draw();

        // reset color
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        const int pad = iconSize/2 - DemoArtwork::ico1Width/2;

        img1.drawAt(pad, pad);
        img2.drawAt(pad, pad + 3 + iconSize);
        img3.drawAt(pad, pad + 6 + iconSize*2);
        img4.drawAt(pad, pad + 9 + iconSize*3);
        img5.drawAt(pad, pad + 12 + iconSize*4);

#if 0
        // draw some text
        nvg.beginFrame(this);

        nvg.fontSize(23.0f);
        nvg.textAlign(NanoVG::ALIGN_LEFT|NanoVG::ALIGN_TOP);
        //nvg.textLineHeight(20.0f);

        nvg.fillColor(220,220,220,220);
        nvg.textBox(10, 420, iconSize, "Haha,", nullptr);
        nvg.textBox(15, 440, iconSize, "Look!", nullptr);

        nvg.endFrame();
#endif
    }

    bool onMouse(const MouseEvent& ev) override
    {
        if (ev.button != 1 || ! ev.press)
            return false;
        if (! contains(ev.pos))
            return false;

        const int iconSize = bgIcon.getWidth();

        for (int i=0; i<kPageCount; ++i)
        {
            bgIcon.setY(i*iconSize + i*3);

            if (bgIcon.contains(ev.pos))
            {
                curPage = i;
                callback->curPageChanged(i);
                repaint();
                break;
            }
        }

        return true;
    }

    bool onMotion(const MotionEvent& ev) override
    {
        if (contains(ev.pos))
        {
            const int iconSize = bgIcon.getWidth();

            for (int i=0; i<kPageCount; ++i)
            {
                bgIcon.setY(i*iconSize + i*3);

                if (bgIcon.contains(ev.pos))
                {
                    if (curHover == i)
                        return true;

                    curHover = i;
                    repaint();
                    return true;
                }
            }

            if (curHover == -1)
                return true;

            curHover = -1;
            repaint();
            return true;
        }
        else
        {
            if (curHover == -1)
                return false;

            curHover = -1;
            repaint();
            return true;
        }
    }

    void onResize(const ResizeEvent& ev) override
    {
        const uint width  = ev.size.getWidth();
        const uint height = ev.size.getHeight();

        bgIcon.setWidth(width-4);
        bgIcon.setHeight(width-4);

        lineSep.setStartPos(width, 0);
        lineSep.setEndPos(width, height);
    }

private:
    Callback* const callback;
    int curPage, curHover;
    Rectangle<double> bgIcon;
    Line<int> lineSep;
    OpenGLImage img1, img2, img3, img4, img5;

#if 0
    // for text
    NanoVG nvg;D
    NanoVG::FontId font;
#endif
};

// --------------------------------------------------------------------------------------------------------------------
// Main Demo Window, having a left-side tab-like widget and main area for current widget

class DemoWindow : public StandaloneWindow,
                   public LeftSideWidget::Callback
{
    static const int kSidebarWidth = 81;

public:
    static constexpr const char* const kExampleWidgetName = "Demo";

    DemoWindow(Application& app)
        : StandaloneWindow(app),
          wColor(this),
          wImages(this),
          wRects(this),
          wShapes(this),
          wLeft(this, this),
          curWidget(nullptr)
    {
        wColor.hide();
        wImages.hide();
        wRects.hide();
        wShapes.hide();
//         wText.hide();
//         //wPerf.hide();

        wColor.setAbsoluteX(kSidebarWidth);
        wImages.setAbsoluteX(kSidebarWidth);
        wRects.setAbsoluteX(kSidebarWidth);
        wShapes.setAbsoluteX(kSidebarWidth);
//         wText.setAbsoluteX(kSidebarWidth);
        wLeft.setAbsolutePos(2, 2);
//         wPerf.setAbsoluteY(5);

        setSize(600, 500);
        setTitle("DGL Demo");

        curPageChanged(0);
    }

protected:
    void curPageChanged(int curPage) override
    {
        if (curWidget != nullptr)
            curWidget->hide();

        switch (curPage)
        {
        case 0:
            curWidget = &wColor;
            break;
        case 1:
            curWidget = &wImages;
            break;
        case 2:
            curWidget = &wRects;
            break;
        case 3:
            curWidget = &wShapes;
            break;
//         case 4:
//             curWidget = &wText;
//             break;
        default:
            curWidget = nullptr;
            break;
        }

        if (curWidget != nullptr)
            curWidget->show();
    }

    void onDisplay() override
    {
    }

    void onReshape(uint width, uint height) override
    {
        StandaloneWindow::onReshape(width, height);

        if (width < kSidebarWidth)
            return;

        Size<uint> size(width-kSidebarWidth, height);
        wColor.setSize(size);
        wImages.setSize(size);
        wRects.setSize(size);
        wShapes.setSize(size);
//         wText.setSize(size);

        wLeft.setSize(kSidebarWidth-4, height-4);
        //wRezHandle.setAbsoluteX(width-wRezHandle.getWidth());
        //wRezHandle.setAbsoluteY(height-wRezHandle.getHeight());

//         wPerf.setAbsoluteX(width-wPerf.getWidth()-5);
    }

private:
    ExampleColorSubWidget wColor;
    ExampleImagesSubWidget wImages;
    ExampleRectanglesSubWidget wRects;
    ExampleShapesSubWidget wShapes;
//     ExampleTextWidget wText;
    LeftSideWidget wLeft;
    //ResizeHandle wRezHandle;
//     NanoPerfWidget wPerf;

    Widget* curWidget;
};

// --------------------------------------------------------------------------------------------------------------------
// Special handy function that runs a StandaloneWindow inside the function scope

template <class ExampleWidgetStandaloneWindow>
void createAndShowExampleWidgetStandaloneWindow(Application& app)
{
    ExampleWidgetStandaloneWindow swin(app);
    swin.setSize(600, 500);
    swin.setTitle(ExampleWidgetStandaloneWindow::kExampleWidgetName);
    swin.show();
    app.exec();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL

int main(int argc, char* argv[])
{
    USE_NAMESPACE_DGL;
    using DGL_NAMESPACE::Window;

    Application app;

    if (argc > 1)
    {
        // TODO images, text

        /**/ if (std::strcmp(argv[1], "color") == 0)
            createAndShowExampleWidgetStandaloneWindow<ExampleColorStandaloneWindow>(app);
        else if (std::strcmp(argv[1], "images") == 0)
            createAndShowExampleWidgetStandaloneWindow<ExampleImagesStandaloneWindow>(app);
        else if (std::strcmp(argv[1], "rectangles") == 0)
            createAndShowExampleWidgetStandaloneWindow<ExampleRectanglesStandaloneWindow>(app);
        else if (std::strcmp(argv[1], "shapes") == 0)
            createAndShowExampleWidgetStandaloneWindow<ExampleShapesStandaloneWindow>(app);
        else
            d_stderr2("Invalid demo mode, must be one of: color, rectangles, shapes");
    }
    else
    {
        createAndShowExampleWidgetStandaloneWindow<DemoWindow>(app);
    }

    return 0;
}

// --------------------------------------------------------------------------------------------------------------------
