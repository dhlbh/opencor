#ifndef QTMMLWIDGETGLOBAL_H
#define QTMMLWIDGETGLOBAL_H

#ifdef QtMmlWidget_PLUGIN
    #define QTMMLWIDGET_EXPORT Q_DECL_EXPORT
#else
    #define QTMMLWIDGET_EXPORT Q_DECL_IMPORT
#endif

#endif