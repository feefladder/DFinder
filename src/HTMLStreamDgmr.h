#ifndef _HTMLSTREAMDGMR_H_
#define _HTMLSTREAMDGMR_H_

#include "ReferenceFinder.h"
#include <stack>

/**********
class HTMLStreamDgmr - a subclass of RefDgmr that writes a PostScript stream of
diagrams.
**********/
class HTMLStreamDgmr : public RefDgmr {
public:
  static double SVGUnit;          // size in points of a unit square
  static const XYRect sPSPageSize;// TODO: remove

  HTMLStreamDgmr(std::ostream& os);

  // Write to stream
  // void PutMarkList(const XYPt& pp, std::vector<RefMark*>& vm);
  // void PutLineList(const XYLine& ll, std::vector<RefLine*>& vl);
  void PutDividedRefList(int total, std::vector<std::pair<int,RefLine*> > vls);
  // void PutLineDiagrams(RefLine* vr);

private:
  std::ostream* mStream;
  XYPt mSVGOrigin;          // current loc of the origin in PS units
  std::stack<std::string> mClosingTags;
    
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
  void DrawPt(const XYPt& aPt, PointStyle pstyle);
  void DrawLine(const XYPt& fromPt, const XYPt& toPt, LineStyle lstyle);
  void DrawPoly(const std::vector<XYPt>& poly, PolyStyle pstyle);
  void DrawLabel(const XYPt& aPt, const std::string& aString, LabelStyle lstyle);
  void DrawFoldAndUnfoldArrow(const XYPt& fromPt, const XYPt& toPt);
  void GroupAndLabel(RefBase &refbase);

  // HTMLStreamDgmr - specific stuff
  void MakeHeader();
  void SetPointStyle(PointStyle pstyle);
  void SetLineStyle(LineStyle lstyle);
  void SetPolyStyle(PolyStyle pstyle);
  void SetLabelStyle(LabelStyle lstyle);
  void Indent();
  void AddClosingTag(std::string tag);
  void CloseTag();

  void NewLine();
  // void MoveOriginRight(double d, double linespace);
  // template <class R>
  // void PutRefList(const typename R::bare_t& ar, std::vector<R*>& vr);
};

#endif // _HTMLSTREAMDGMR_H_