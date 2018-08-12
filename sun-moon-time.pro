#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T12:27:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT  +=  webkitwidgets multimedia

TARGET = sun-moon-time
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    aaplus/AAAberration.cpp \
    aaplus/AAAngularSeparation.cpp \
    aaplus/AABinaryStar.cpp \
    aaplus/AACoordinateTransformation.cpp \
    aaplus/AADate.cpp \
    aaplus/AADiameters.cpp \
    aaplus/AADynamicalTime.cpp \
    aaplus/AAEarth.cpp \
    aaplus/AAEaster.cpp \
    aaplus/AAEclipses.cpp \
    aaplus/AAEclipticalElements.cpp \
    aaplus/AAElementsPlanetaryOrbit.cpp \
    aaplus/AAElliptical.cpp \
    aaplus/AAEquationOfTime.cpp \
    aaplus/AAEquinoxesAndSolstices.cpp \
    aaplus/AAFK5.cpp \
    aaplus/AAGalileanMoons.cpp \
    aaplus/AAGlobe.cpp \
    aaplus/AAIlluminatedFraction.cpp \
    aaplus/AAInterpolate.cpp \
    aaplus/AAJewishCalendar.cpp \
    aaplus/AAJupiter.cpp \
    aaplus/AAKepler.cpp \
    aaplus/AAMars.cpp \
    aaplus/AAMercury.cpp \
    aaplus/AAMoon.cpp \
    aaplus/AAMoonIlluminatedFraction.cpp \
    aaplus/AAMoonMaxDeclinations.cpp \
    aaplus/AAMoonNodes.cpp \
    aaplus/AAMoonPerigeeApogee.cpp \
    aaplus/AAMoonPhases.cpp \
    aaplus/AAMoslemCalendar.cpp \
    aaplus/AANearParabolic.cpp \
    aaplus/AANeptune.cpp \
    aaplus/AANodes.cpp \
    aaplus/AANutation.cpp \
    aaplus/AAParabolic.cpp \
    aaplus/AAParallactic.cpp \
    aaplus/AAParallax.cpp \
    aaplus/AAPhysicalJupiter.cpp \
    aaplus/AAPhysicalMars.cpp \
    aaplus/AAPhysicalMoon.cpp \
    aaplus/AAPhysicalSun.cpp \
    aaplus/AAPlanetaryPhenomena.cpp \
    aaplus/AAPlanetPerihelionAphelion.cpp \
    aaplus/AAPluto.cpp \
    aaplus/AAPrecession.cpp \
    aaplus/AARefraction.cpp \
    aaplus/AARiseTransitSet.cpp \
    aaplus/AASaturn.cpp \
    aaplus/AASaturnMoons.cpp \
    aaplus/AASaturnRings.cpp \
    aaplus/AASidereal.cpp \
    aaplus/AAStellarMagnitudes.cpp \
    aaplus/AASun.cpp \
    aaplus/AAUranus.cpp \
    aaplus/AAVenus.cpp \
    aaplus/stdafx.cpp \
    tithi.cpp \
    calendar.cpp \
    computings.cpp \
    dialogsettings.cpp \
    dialogtithi.cpp \
    dialogtableview.cpp \
    dialogabout.cpp \
    dialogvideosviewer.cpp

HEADERS  += mainwindow.h \
    aaplus/AA+.h \
    aaplus/AA2DCoordinate.h \
    aaplus/AA3DCoordinate.h \
    aaplus/AAAberration.h \
    aaplus/AAAngularSeparation.h \
    aaplus/AABinaryStar.h \
    aaplus/AACoordinateTransformation.h \
    aaplus/AADate.h \
    aaplus/AADiameters.h \
    aaplus/AADynamicalTime.h \
    aaplus/AAEarth.h \
    aaplus/AAEaster.h \
    aaplus/AAEclipses.h \
    aaplus/AAEclipticalElements.h \
    aaplus/AAElementsPlanetaryOrbit.h \
    aaplus/AAElliptical.h \
    aaplus/AAEquationOfTime.h \
    aaplus/AAEquinoxesAndSolstices.h \
    aaplus/AAFK5.h \
    aaplus/AAGalileanMoons.h \
    aaplus/AAGlobe.h \
    aaplus/AAIlluminatedFraction.h \
    aaplus/AAInterpolate.h \
    aaplus/AAJewishCalendar.h \
    aaplus/AAJupiter.h \
    aaplus/AAKepler.h \
    aaplus/AAMars.h \
    aaplus/AAMercury.h \
    aaplus/AAMoon.h \
    aaplus/AAMoonIlluminatedFraction.h \
    aaplus/AAMoonMaxDeclinations.h \
    aaplus/AAMoonNodes.h \
    aaplus/AAMoonPerigeeApogee.h \
    aaplus/AAMoonPhases.h \
    aaplus/AAMoslemCalendar.h \
    aaplus/AANearParabolic.h \
    aaplus/AANeptune.h \
    aaplus/AANodes.h \
    aaplus/AANutation.h \
    aaplus/AAParabolic.h \
    aaplus/AAParallactic.h \
    aaplus/AAParallax.h \
    aaplus/AAPhysicalJupiter.h \
    aaplus/AAPhysicalMars.h \
    aaplus/AAPhysicalMoon.h \
    aaplus/AAPhysicalSun.h \
    aaplus/AAPlanetaryPhenomena.h \
    aaplus/AAPlanetPerihelionAphelion.h \
    aaplus/AAPluto.h \
    aaplus/AAPrecession.h \
    aaplus/AARefraction.h \
    aaplus/AARiseTransitSet.h \
    aaplus/AASaturn.h \
    aaplus/AASaturnMoons.h \
    aaplus/AASaturnRings.h \
    aaplus/AASidereal.h \
    aaplus/AAStellarMagnitudes.h \
    aaplus/AASun.h \
    aaplus/AAUranus.h \
    aaplus/AAVenus.h \
    aaplus/stdafx.h \
    tithi.h \
    calendar.h \
    computings.h \
    dialogsettings.h \
    dialogtithi.h \
    dialogtableview.h \
    dialogabout.h \
    dialogvideosviewer.h

FORMS    += mainwindow.ui \
    dialogsettings.ui \
    dialogtithi.ui \
    dialogtableview.ui \
    dialogabout.ui \
    dialogvideosviewer.ui

RESOURCES += \
    resources.qrc

win32
{
   RC_FILE = sun-moon-time.rc
}

OTHER_FILES += \
    sun-moon-time.rc \
    version_history.txt

DISTFILES += \
    description \
    journal
