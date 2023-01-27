QT       += core gui sql network printsupport multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 link_pkgconfig

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialogforaddbtn.cpp \
    dialogfordeletebtn.cpp \
    dialogforeditbtn.cpp \
    main.cpp \
    mainwindow.cpp \
    rinfo.cpp \
    route.cpp \
    routedata.cpp \
    statusofips.cpp \
    threads.cpp

HEADERS += \
    common.h \
    dialogforaddbtn.h \
    dialogfordeletebtn.h \
    dialogforeditbtn.h \
    mainwindow.h \
    rinfo.h \
    route.h \
    routedata.h \
    statusofips.h \
    threads.h

FORMS += \
    dialogforaddbtn.ui \
    dialogfordeletebtn.ui \
    dialogforeditbtn.ui \
    mainwindow.ui \
    statusofips.ui

#PKGCONFIG += gstreamer-1.0 \
#            glib-2.0 \
#            gobject-2.0 \
#            gio-2.0

PKGCONFIG += gstreamer-1.0 glib-2.0 gobject-2.0 gstreamer-app-1.0 gstreamer-pbutils-1.0


INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include/
              /usr/include/gstreamer-1.0/
              /usr/include/glib-2.0/



#LIBS += glib-2.0 \
#        gstnet-1.0 \
#        gstbase-1.0 \
#        gstcheck-1.0 \
#        gstreamer-1.0 \
#        gstcontroller-1.0






# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
