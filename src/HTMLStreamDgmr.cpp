#include "HTMLStreamDgmr.h"

#include <sstream>

/**********
class HTMLStreamDgmr - a specialization of RefDgmr that writes a HTML page
stream of diagrams.
**********/

/* Notes on class HTMLStreamDgmr.
This class is used in the command-line version of ReferenceFinder to create
a HTML graphics file of folding diagrams. It's also a good model for how
to implement a true graphic outputter.
*/

void HTMLStreamDgmr::Indent()
{
  for(size_t i=0; i< mClosingTags.size(); i++)
    (*mStream) << "    ";
}

void HTMLStreamDgmr::AddClosingTag(std::string tag)
{
  mClosingTags.emplace("</" + tag + ">");
}

void HTMLStreamDgmr::CloseTag()
{
  std::string tag = mClosingTags.top();
  mClosingTags.pop();

  Indent();
  (*mStream) << tag << std::endl;

}

/*****
HTMLStreamDgmr static member initialization
*****/
double HTMLStreamDgmr::SVGUnit = 150;    // 72 pts = 1 inch, 1 unit = 64 pts, fits 7 dgms
// const XYRect HTMLStreamDgmr::sPSPageSize(40, 40, 572, 752);  // printable area on the page


/*****
Constructor
*****/
HTMLStreamDgmr::HTMLStreamDgmr(std::ostream& os) :
   mStream(&os)
{
}

/*****
Make the HTML header
*****/
void HTMLStreamDgmr::MakeHeader(){
  (*mStream)<<"<!DOCTYPE html>\n<html>\n<head>\n  <svg width='0' height='0'>\n    <defs>\n        <marker id='arrow' markerWidth='10' markerHeight='10' refX='9' refY='3' orient='auto' markerUnits='strokeWidth'>\n"
  "        <path d='M0,0 L9,3 L0,6' stroke='green' fill='none'/>\n        </marker>\n        <marker id='unfold_arrow' markerWidth='10' markerHeight='10' refX='0' refY='3' orient='auto' markerUnits='strokeWidth'>"
  "        <path d='M0,3 L9,0 L9,6 z' stroke='green' fill='#fd7' stroke-linejoin='round' />\n        </marker>\n      </defs>\n"
  "  </svg>\n</head>\n<body>";
}

/***
 * Group and label the diagram
 */
void HTMLStreamDgmr::GroupAndLabel(RefBase &refbase)
{
  Indent();
  (*mStream) << "<g class='diagram' transform='translate("<< mSVGOrigin.x <<","<< mSVGOrigin.y<<")' >" << std::endl;
  AddClosingTag("g");
  Indent();
  (*mStream) << "<title>"; refbase.PutHowto(*mStream); (*mStream) << "</title>" << std::endl;
}

/*****
Stream output for a SVG point TODO: make redundant
*****/
std::ostream& operator<<(std::ostream& os, const HTMLStreamDgmr::SVGPt& pp)
{
  return os << pp.px << " " << pp.py;
}

/*****
Set the current graphics state to the given PointStyle.
*****/
void HTMLStreamDgmr::SetPointStyle(PointStyle pstyle)
{
  switch (pstyle) {
    case POINTSTYLE_NORMAL:
      (*mStream) << "stroke='black' stroke-width='1'";
      break;
    case POINTSTYLE_HILITE:
      (*mStream) << "stroke='rgb(128,64,64)' stroke-width='3'";
      break;
    case POINTSTYLE_ACTION:
      (*mStream) << "stroke='rgb(128,0,0)' stroke-width='3'";
      break;
  }
}


/*****
Set the current graphics state to the given LineStyle
*****/
void HTMLStreamDgmr::SetLineStyle(LineStyle lstyle)
{
  switch (lstyle) {
    case LINESTYLE_CREASE:
      (*mStream) << "stroke='darkgray' stroke-width='.5'";
      break;
    case LINESTYLE_EDGE:
      (*mStream) << "stroke='black' stroke-width='2'";
      break;
    case LINESTYLE_HILITE:
      (*mStream) << "stroke='darkmagenta' stroke-width='2'";
      break;
    case LINESTYLE_VALLEY:
      (*mStream) << "stroke='green' stroke-width='.5' stroke-dasharray='2'";
      break;
    case LINESTYLE_MOUNTAIN:
      (*mStream) << "stroke='green' stroke-width='.5' stroke-dasharray='3 2 2 2'";
      break;
    case LINESTYLE_ARROW:
      (*mStream) << "stroke='darkgreen' stroke-width='.5'";
      break;
  }
}


/*****
Set the current graphics state to the given PolyStyle
*****/
void HTMLStreamDgmr::SetPolyStyle(PolyStyle pstyle)
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
void HTMLStreamDgmr::SetLabelStyle(LabelStyle lstyle)
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
HTMLStreamDgmr::SVGPt HTMLStreamDgmr::ToSVG(const XYPt& aPt)
{
  return SVGPt(SVGUnit * aPt.x, SVGUnit * aPt.y);
}

/****
Draw a fold-andunfold arrow
****/
void  HTMLStreamDgmr::DrawFoldAndUnfoldArrow(const XYPt& fromPt, const XYPt& toPt)
{
  SVGPt fPt = ToSVG(fromPt);
  SVGPt tPt = ToSVG(toPt);
  //TODO: make this a nice curved arrow
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

  Indent();
  (*mStream) << "<path d='M "<<fPt.px<<" "<<fPt.py<<"Q "<<cPt.px<<" "<<cPt.py<<" "<<tPt.px<<" "<<tPt.py
    <<"' fill='none' stroke='green' stroke-width='1'"
    <<"marker-start='url(#unfold_arrow)' marker-end='url(#arrow)'/>"<< std::endl;
}

/*****
Draw an SVG point in the indicated style.
*****/
void HTMLStreamDgmr::DrawPt(const XYPt& aPt, PointStyle pstyle)
{
  SVGPt sPt = ToSVG(aPt);

  Indent();
  (*mStream) << "<circle r='1' cx='"<<sPt.px<<"' cy='"<<sPt.py<<"'";
  SetPointStyle(pstyle);
  (*mStream) << "/>"<< std::endl;
}


/*****
Draw a SVG line in the indicated style.
*****/
void HTMLStreamDgmr::DrawLine(const XYPt& fromPt, const XYPt& toPt, 
  LineStyle lstyle)
{
  SVGPt fPt = ToSVG(fromPt);
  SVGPt tPt = ToSVG(toPt);

  Indent();
  (*mStream) << "<line x1='"<<fPt.px<<"' y1='"<<fPt.py<<"' x2='"<<tPt.px<<"' y2='"<<tPt.py<<"'";
  SetLineStyle(lstyle);
  (*mStream) << "/>" << std::endl;
}


/*****
Fill and stroke the given poly in the indicated style.
*****/
void HTMLStreamDgmr::DrawPoly(const std::vector<XYPt>& poly, PolyStyle pstyle)
{
  // Since this is the 
  Indent();
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
void HTMLStreamDgmr::DrawLabel(const XYPt& aPt, const std::string& aString, LabelStyle lstyle)
{
  // SetLabelStyle(lstyle);
  SVGPt sPt = ToSVG(aPt);
  Indent();
  (*mStream)<<"<text x='"<<sPt.px<<"' y='"<<sPt.py<<"'>"<<aString<<"</text>"<< std::endl;
}


/*****
newline
*****/

void HTMLStreamDgmr::NewLine()
{
  
  (*mStream)<<"</p>\n<p>";
}

/*****
Draw a set of marks or lines to a PostScript stream, showing distance and rank
for each sequence.
*****/
// template <class R>
// void HTMLStreamDgmr::PutRefList(const typename R::bare_t& ar, std::vector<R*>& vr)
// {
//   ReferenceFinder::sClarifyVerbalAmbiguities = false;
//   ReferenceFinder::sAxiomsInVerbalDirections = false;

//   // Put some comments so our readers are happy 
//   MakeHeader();
//   // (*mStream) << "/Times-Roman findfont 12 scalefont setfont" << std::endl;
//   // (*mStream) << "0 setgray" << std::endl;
//   NewLine();
//   (*mStream)<<"ReferenceFinder 4.0 by Robert J. Lang";
  
//   // Note the point we're searching for.
//   // (*mStream) << "/Times-Roman findfont 9 scalefont setfont" << std::endl;
//   NewLine();
//   std::stringstream targstr;
//   targstr << "Paper: \\(" << ReferenceFinder::sPaper.mWidthAsText.c_str()
// 	  << " x " << ReferenceFinder::sPaper.mHeightAsText.c_str()
// 	  << "\\), Target: " << ar;
//   // DrawLabel(targstr.str(), LABELSTYLE_NORMAL);
  
//   // Go through our list and draw all the diagrams in a single row. 
//   for (size_t irow = 0; irow < vr.size(); irow++) {
//     NewLine();
//     vr[irow]->BuildDiagrams();
//     mSVGOrigin.x = 0;//sPSPageSize.bl.x;
//     for (size_t icol = 0; icol < RefBase::sDgms.size(); icol++) {
//       RefBase::DrawDiagram(*this, RefBase::sDgms[icol]);
//       mSVGOrigin.x += 1.2 * ReferenceFinder::sPaper.mWidth * SVGUnit;
//     };
    
//     // Also put the text description below the diagrams   
//     mSVGOrigin.x = 0;//sPSPageSize.bl.x;
//     NewLine();
//     ostd::stringstream sd;
//     vr[irow]->PutDistanceAndRank(sd, ar);
//     // DrawLabel(sd.str(), LABELSTYLE_NORMAL);
//     for (size_t i = 0; i < RefBase::sSequence.size(); i++) {
//       mSVGOrigin.x = 0;// sPSPageSize.bl.x;
//       ostd::stringstream s;
//       if (RefBase::sSequence[i]->PutHowto(s)) {
//         NewLine();
//         s << ".";
//         // DrawLabel(s.str(), LABELSTYLE_NORMAL);
//       }
//     }
//   }
  
//   // Close the file.  
//   // (*mStream) << "showpage" << std::endl;
//   // (*mStream) << "%%Trailer" << std::endl;
//   // (*mStream) << "%%Pages: " << mPSPageCount << std::endl;
// }

/*****
Specialized version for division into nths
Draw a set of marks or lines to a PostScript stream, showing distance and rank
for each sequence.
*****/
void HTMLStreamDgmr::PutDividedRefList(int total, std::vector<std::pair<int,RefLine*>> vls)
{
  ReferenceFinder::sClarifyVerbalAmbiguities = false;
  ReferenceFinder::sAxiomsInVerbalDirections = false;

  // Put some initial setup information 
  MakeHeader();
  (*mStream) << "ReferenceFinder 4.0 by Robert J. Lang, hacky divisionfinder version by Joep Gevaert" << std::endl;
  
  // Note the point we're searching for.
  // (*mStream) << "/Times-Roman findfont 9 scalefont setfont" << std::endl;
  // NewLine();
  std::stringstream targstr;
  targstr << "Paper: (" << ReferenceFinder::sPaper.mWidthAsText.c_str()
	  << " x " << ReferenceFinder::sPaper.mHeightAsText.c_str()
	  << "), Target: " << total;
  // DrawLabel(targstr.str(), LABELSTYLE_NORMAL);
  
  // Go through our list and draw all the diagrams in a single row.
  (*mStream) <<"<svg height='"<<SVGUnit*ReferenceFinder::sPaper.mHeight*1.2<<"px' width='"<<10*SVGUnit*ReferenceFinder::sPaper.mWidth*1.2<<"px'>" << std::endl;
  AddClosingTag("svg");
  for (size_t irow = 0; irow < 1; irow++) {
    XYLine ar(double(vls[irow].first)/double(total));
    // std::vector<int> cycle = find_cycle(total,vls[irow].first);
    // NewLine();
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
  // DrawLabel(XYPt(0),"poep", LABELSTYLE_HILITE);
  }
  
  

  // Close the file.  
  (*mStream)<<"</body>\n</html>"<< std::endl;

}

/*****
Write the PostScript code that draws folding sequences for a list of marks.
*****/

// void HTMLStreamDgmr::PutMarkList(const XYPt& pp, std::vector<RefMark*>& vm)
// {
//   PutRefList(pp, vm);
// }


// /*****
// Write the PostScript code that draws folding sequences for a list of lines.
// *****/
// void HTMLStreamDgmr::PutLineList(const XYLine& ll, std::vector<RefLine*>& vl)
// {
//   PutRefList(ll, vl);
// }
