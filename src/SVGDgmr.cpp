#include "SVGDgmr.h"

// #include <sstream>

/**********
class SVGDgmr - a specialization of RefDgmr that writes a HTML page
stream of diagrams.
**********/

/* Notes on class SVGDgmr.
This class is used in the command-line version of ReferenceFinder to create
a HTML graphics file of folding diagrams. It's also a good model for how
to implement a true graphic outputter.
*/
double SVGDgmr::SVGUnit = 150;    // 72 pts = 1 inch, 1 unit = 64 pts, fits 7 dgms
// const XYRect SVGDgmr::sPSPageSize(40, 40, 572, 752);  // printable area on the page
/***
 * Group and label the diagram
 */
void SVGDgmr::PutTitle(RefBase &refbase)
{
  (*mStream) << "<title>"; refbase.PutHowto(*mStream); (*mStream) << "</title>" << std::endl;
}

/*****
Set the current graphics state to the given PointStyle.
*****/
void SVGDgmr::SetPointStyle(PointStyle pstyle)
{
  switch (pstyle) {
    case POINTSTYLE_NORMAL:
      (*mStream) << " class='pt_normal' ";
      break;
    case POINTSTYLE_HILITE:
      (*mStream) << " class='pt_hilite' ";
      break;
    case POINTSTYLE_ACTION:
      (*mStream) << " class='pt_action' ";
      break;
  }
}

/*****
Set the current graphics state to the given LineStyle
*****/
void SVGDgmr::SetLineStyle(LineStyle lstyle)
{
  switch (lstyle) {
    case LINESTYLE_CREASE:
      (*mStream) << " class='l_crease' ";//stroke='darkgray' stroke-width='.5'";
      break;
    case LINESTYLE_EDGE:
      (*mStream) << " class='l_edge' ";//stroke='black' stroke-width='2'";
      break;
    case LINESTYLE_HILITE:
      (*mStream) << " class='l_hilite' ";//stroke='darkmagenta' stroke-width='2'";
      break;
    case LINESTYLE_VALLEY:
      (*mStream) << " class='l_valley' ";//stroke='green' stroke-width='.5' stroke-dasharray='2'";
      break;
    case LINESTYLE_MOUNTAIN:
      (*mStream) << " class='l_mountain' ";//stroke='green' stroke-width='.5' stroke-dasharray='3 2 2 2'";
      break;
    case LINESTYLE_ARROW:
      (*mStream) << " class='l_arrow' ";//stroke='darkgreen' stroke-width='.5'";
      break;
  }
}


/*****
Set the current graphics state to the given PolyStyle
*****/
std::string SVGDgmr::SetPolyStyle(PolyStyle pstyle)
{
  switch (pstyle) {
    case POLYSTYLE_WHITE:
      (*mStream) << "fill='#fd9'";
      break;
    case POLYSTYLE_COLORED:
      (*mStream) << "fill='darkblue'";
      break;
    case POLYSTYLE_ARROW:
      (*mStream) << "fill='green'";
      break;
  }
}


/*****
Set the current graphics state to the given LabelStyle
*****/
std::string SVGDgmr::SetLabelStyle(LabelStyle lstyle)
{
  switch (lstyle) {
    case LABELSTYLE_NORMAL:
      (*mStream) << "0 setgray ";
      break;
    case LABELSTYLE_HILITE:
      (*mStream) << ".5 .25 .25 setrgbcolor ";
      break;
    case LABELSTYLE_ACTION:
      (*mStream) << ".5 0 0 setrgbcolor ";
      break;
  }
}


/*****
Coordinate conversion
*****/
SVGDgmr::SVGPt SVGDgmr::ToSVG(const XYPt& aPt)
{
  return SVGPt(SVGUnit * aPt.x, SVGUnit * aPt.y);
}

/****
Draw a fold-andunfold arrow
****/
void SVGDgmr::DrawFoldAndUnfoldArrow(const XYPt& fromPt, const XYPt& toPt)
{
  SVGPt fPt = ToSVG(fromPt);
  SVGPt tPt = ToSVG(toPt);
  XYPt mp = MidPoint(fromPt,toPt);
  XYPt mu = fromPt-toPt;
  XYPt mup = 0.3 * mu.Rotate90();  // std::vector from midpt to center of curvature
  
  // Compute the control point. There are two possible choices.
  // We'll want the bulge of the arc to always be toward the inside of the square,
  // i.e., closer to the middle of the square, so we pick the value of the point
  // that's closest.
  XYPt sqmp = MidPoint(ReferenceFinder::sPaper.mBotLeft, 
    ReferenceFinder::sPaper.mTopRight);
  XYPt cp1 = mp + mup;
  XYPt cp2 = mp - mup;
  XYPt cp = (cp1 - sqmp).Mag() < (cp2 - sqmp).Mag() ? cp1 : cp2;
  // mp-=mu.Rotate90()*.25;
  SVGPt cPt = ToSVG(cp);
  (*mStream) "<path d='M "<<fPt.px<<" "<<fPt.py<<"Q "<<cPt.px<<" "<<cPt.py<<" "<<tPt.px<<" "<<tPt.py<<"' style='fold-unfold-arrow'/>"<< std::endl;
}

/*****
Draw an SVG point in the indicated style.
*****/
std::string SVGDgmr::DrawPt(const XYPt& aPt, PointStyle pstyle)
{
  SVGPt sPt = ToSVG(aPt);
  (*mStream) << "<circle r='1' cx='"<<sPt.px<<"' cy='"<<sPt.py<<"'";
  SetPointStyle(pstyle);
  (*mStream) << "/>"<< std::endl;
}


/*****
Draw a SVG line in the indicated style.
*****/
std::string SVGDgmr::DrawLine(const XYPt& fromPt, const XYPt& toPt, 
  LineStyle lstyle)
{
  SVGPt fPt = ToSVG(fromPt);
  SVGPt tPt = ToSVG(toPt);
  (*mStream) << "<line x1='"<<fPt.px<<"' y1='"<<fPt.py<<"' x2='"<<tPt.px<<"' y2='"<<tPt.py<<"'";
  SetLineStyle(lstyle);
  (*mStream) << "/>" << std::endl;
}


/*****
Fill and stroke the given poly in the indicated style.
*****/
std::string SVGDgmr::DrawPoly(const std::vector<XYPt>& poly, PolyStyle pstyle)
{
  // Since this is the 
  (*mStream) << "<path d='M" << ToSVG(poly[poly.size()-1]);
  for (size_t i = 0; i < poly.size(); i++)
    (*mStream) << " L "<< ToSVG(poly[i]);
  (*mStream) << "'";

  // Fill the poly
  SetPolyStyle(pstyle);
  // (*mStream) << "fill grestore " << std::endl;
  
  // Stroke the poly
  switch (pstyle) {
    case POLYSTYLE_WHITE:
    case POLYSTYLE_COLORED:
      SetLineStyle(LINESTYLE_EDGE);
      break;
    case POLYSTYLE_ARROW:
      SetLineStyle(LINESTYLE_ARROW);
      break;
  };
  (*mStream) << "/>" << std::endl;
}


/*****
Draw a text label at the point aPt in the indicated style
*****/
std::string SVGDgmr::DrawLabel(const XYPt& aPt, const std::string& aString, LabelStyle lstyle)
{
  // SetLabelStyle(lstyle);
  SVGPt sPt = ToSVG(aPt);
  Indent();
  (*mStream) << "<text x='"<<sPt.px<<"' y='"<<sPt.py<<"'>"<<aString<<"</text>"<< std::endl;
}

struct instruction {
    std::string description
    std::vector<std::pair<std::string,std::string>>> instructions;
};

/*****
Specialized version for division into nths
Draw a set of marks or lines to a PostScript stream, showing distance and rank
for each sequence.
*****/

std::vector<std::pair<std::string,std::string>> SVGDgmr::PutDividedRefList(int total, std::vector<std::pair<int,RefLine*>> vls)
{
  const size_t irow = 0;
  // for (size_t irow = 0; irow < 1; irow++) { //increase for more
    XYLine ar(double(vls[irow].first)/double(total));
    vls[irow].second->BuildDiagrams(true);
    mSVGOrigin.x = 25;
    mSVGOrigin.y = 25;
    for (size_t icol = 0; icol < RefBase::sDgms.size(); icol++) {
      RefBase::DrawDiagram(*this, RefBase::sDgms[icol]);
      CloseTag(); //g
      mSVGOrigin.x += 1.2 * ReferenceFinder::sPaper.mWidth * SVGUnit;
    };
    CloseTag();
    // (*mStream) << "</svg>";
    // Also put the text description below the diagrams   

    NewLine();
    (*mStream) << "Found a solution for: " << vls[irow].first << "/" << total << std::endl;
    NewLine();
    vls[irow].second->PutDistanceAndRank(*mStream, ar);
    
    for (size_t i = 0; i < RefBase::sSequence.size(); i++) {
      if (RefBase::sSequence[i]->PutHowto(*mStream)) {
        NewLine();
      }
    }
    // NewLine();
    RefLine* vr = new RefLine(ReferenceFinder::FoldCycles(vls[irow].first,total,vls[irow].second->mRank));
    vr->BuildDiagrams(false);
    mSVGOrigin.x = 0;
    mSVGOrigin.y = 25;
    size_t cols = 10;
    size_t rows = total/cols+1;
    (*mStream) << "<svg width='"<<(cols-.2)*SVGUnit*ReferenceFinder::sPaper.mWidth*1.2+50<<"px' height='"<<rows*SVGUnit*ReferenceFinder::sPaper.mHeight*1.2+50<<"px'>"<< std::endl;
    AddClosingTag("svg");
    for (size_t icol = 0; icol < RefBase::sDgms.size(); icol++) {
      RefBase::DrawDiagram(*this, RefBase::sDgms[icol]);
      CloseTag(); //g
      if(icol%cols!=cols-1) {
        mSVGOrigin.x += 1.2 * ReferenceFinder::sPaper.mWidth * SVGUnit;
      } else {
        mSVGOrigin.x = 0;
        mSVGOrigin.y += 1.2 * ReferenceFinder::sPaper.mHeight * SVGUnit;
      }
    }
    CloseTag(); //svg
    
    // (*mStream) << "</svg>" << std::endl;
    // Also put the text description below the diagrams   
    NewLine();
    for (size_t i = 0; i < RefBase::sSequence.size(); i++) {
      if (RefBase::sSequence[i]->PutHowto(*mStream)) {
        NewLine();
      }
    }
  // }
}