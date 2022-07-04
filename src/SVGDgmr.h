#ifndef _SVGDGMR_H_
#define _SVGDGMR_H_

#include "ReferenceFinder.h"
#include <sstream>
#include <stack>

struct instruction {
    std::string description;
    std::vector<std::string> diagrams;
    std::vector<std::string> verbal;
};

/**********
class SVGDgmr - a subclass of RefDgmr that writes a PostScript stream of
diagrams.
**********/
class SVGDgmr : public RefDgmr {
public:
  inline static const double SVGUnit = 150;          // size in points of a unit square
  SVGDgmr() {};

  std::ostringstream mStream = std::ostringstream();

  void PutDividedRefList(int total, std::vector<std::pair<int,RefLine*> > vls);
  // void PutLineDiagrams(RefLine* vr);
  instruction PutCycles(int start, int total);


private:
  XYPt mSVGOrigin;          // current loc of the origin in PS units
    
  class SVGPt {
    public:
      double px;
      double py;
      SVGPt(double x, double y) : px(x), py(y) {};
  };
  friend std::ostream& operator<<(std::ostream& os, const SVGPt& pp);

  SVGPt ToSVG(const XYPt& pt);

  // Overridden functions from ancestor class RefDgmr
  void DrawPaper();
  void GroupAndLabel(RefBase& refbase);
  void DrawPt(const XYPt& aPt, PointStyle pstyle);
  void DrawLine(const XYPt& fromPt, const XYPt& toPt, LineStyle lstyle);
  void DrawPoly(const std::vector<XYPt>& poly, PolyStyle pstyle);
  void DrawLabel(const XYPt& aPt, const std::string& aString, LabelStyle lstyle);
  void DrawFoldAndUnfoldArrow(const XYPt& fromPt, const XYPt& toPt);
  

  // SVGDgmr - specific stuff
  void PutTitle(RefBase &refbase);
  void SetPointStyle(PointStyle pstyle);
  void SetLineStyle(LineStyle lstyle);
  void SetPolyStyle(PolyStyle pstyle);
  void SetLabelStyle(LabelStyle lstyle);
};

#endif // _SVGDGMR_H_