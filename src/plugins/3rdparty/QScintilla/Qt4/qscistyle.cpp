// This module implements the QsciStyle class.
//
// Copyright (c) 2012 Riverbank Computing Limited <info@riverbankcomputing.com>
// 
// This file is part of QScintilla.
// 
// This file may be used under the terms of the GNU General Public
// License versions 2.0 or 3.0 as published by the Free Software
// Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
// included in the packaging of this file.  Alternatively you may (at
// your option) use any later version of the GNU General Public
// License if such license has been publicly approved by Riverbank
// Computing Limited (or its successors, if any) and the KDE Free Qt
// Foundation. In addition, as a special exception, Riverbank gives you
// certain additional rights. These rights are described in the Riverbank
// GPL Exception version 1.1, which can be found in the file
// GPL_EXCEPTION.txt in this package.
// 
// If you are unsure which license is appropriate for your use, please
// contact the sales department at sales@riverbankcomputing.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.


#include "Qsci/qscistyle.h"

#include <qapplication.h>

#include "Qsci/qsciscintillabase.h"


// A ctor.
QsciStyle::QsciStyle(int style)
{
    init(style);

    QPalette pal = QApplication::palette();
    setColor(pal.text().color());
    setPaper(pal.base().color());

    setFont(QApplication::font());
    setEolFill(false);
}


// A ctor.
QsciStyle::QsciStyle(int style, const QString &description,
        const QColor &color, const QColor &paper, const QFont &font,
        bool eolFill)
{
    init(style);

    setDescription(description);

    setColor(color);
    setPaper(paper);

    setFont(font);
    setEolFill(eolFill);
}


// Initialisation common to all ctors.
void QsciStyle::init(int style)
{
    // The next style number to allocate.  The initial values corresponds to
    // the amount of space that Scintilla initially creates for styles.
    static int next_style_nr = 63;

    // See if a new style should be allocated.  Note that we allow styles to be
    // passed in that are bigger than STYLE_MAX because the styles used for
    // annotations are allowed to be.
    if (style < 0)
    {
        // Note that we don't deal with the situation where the newly allocated
        // style number has already been used explicitly.
        if (next_style_nr > QsciScintillaBase::STYLE_LASTPREDEFINED)
            style = next_style_nr--;
    }

    style_nr = style;

    // Initialise the minor attributes.
    setTextCase(QsciStyle::OriginalCase);
    setVisible(true);
    setChangeable(true);
    setHotspot(false);
}


// Apply the style to a particular editor.
void QsciStyle::apply(QsciScintillaBase *sci) const
{
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETFORE, style_nr,
            style_color);
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETBACK, style_nr,
            style_paper);
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETFONT, style_nr,
            style_font.family().toAscii().data());
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETSIZE, style_nr,
            style_font.pointSize());
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETBOLD, style_nr,
            style_font.bold());
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETITALIC, style_nr,
            style_font.italic());
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETUNDERLINE, style_nr,
            style_font.underline());
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETEOLFILLED, style_nr,
            style_eol_fill);
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETCASE, style_nr,
            (long)style_case);
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETVISIBLE, style_nr,
            style_visible);
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETCHANGEABLE, style_nr,
            style_changeable);
    sci->SendScintilla(QsciScintillaBase::SCI_STYLESETHOTSPOT, style_nr,
            style_hotspot);
}


// Set the color attribute.
void QsciStyle::setColor(const QColor &color)
{
    style_color = color;
}


// Set the paper attribute.
void QsciStyle::setPaper(const QColor &paper)
{
    style_paper = paper;
}


// Set the font attribute.
void QsciStyle::setFont(const QFont &font)
{
    style_font = font;
}


// Set the eol fill attribute.
void QsciStyle::setEolFill(bool eolFill)
{
    style_eol_fill = eolFill;
}


// Set the text case attribute.
void QsciStyle::setTextCase(QsciStyle::TextCase text_case)
{
    style_case = text_case;
}


// Set the visible attribute.
void QsciStyle::setVisible(bool visible)
{
    style_visible = visible;
}


// Set the changeable attribute.
void QsciStyle::setChangeable(bool changeable)
{
    style_changeable = changeable;
}


// Set the hotspot attribute.
void QsciStyle::setHotspot(bool hotspot)
{
    style_hotspot = hotspot;
}


// Refresh the style.  Note that since we had to add apply() then this can't do
// anything useful so we leave it as a no-op.
void QsciStyle::refresh()
{
}
