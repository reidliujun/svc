#ifndef _TLEVELSSETTINGS_H_
#define _TLEVELSSETTINGS_H_

#include "TfilterSettings.h"

struct TlevelsSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
 typedef int TlevelsSettings::*CRPoint[2];
 static const CRPoint points[10];
 void plot_curve(int p1,int p2,int p3,int p4,unsigned int dp[256]);
 void calcCurve(unsigned int dp[256]);
protected:
 virtual void getMinMax(int id,int &min,int &max);
 virtual const int *getResets(unsigned int pageId);
public:
 TlevelsSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int mode;
 static const char_t *modes[];
 int inMin,gamma,inMax,outMin,outMax,posterize;
 int YmaxDelta,Ythreshold,Ytemporal;
 int onlyLuma;
 int fullY;
 int inAuto;
 int numPoints;
 int point0x,point0y,point1x,point1y,point2x,point2y,point3x,point3y,point4x,point4y,point5x,point5y,point6x,point6y,point7x,point7y,point8x,point8y,point9x,point9y;
 void calcMap(unsigned int map[256],int *divisor);
 void calcMap(unsigned int map[256],int *divisor,int inMin,int inMax);
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
