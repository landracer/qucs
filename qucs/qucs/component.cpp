/***************************************************************************
                          component.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "component.h"

#include <qpoint.h>
#include <qpainter.h>
#include <qstring.h>
#include <qpen.h>



// ***********************************************************************************
// **********                                                               **********
// **********                      class "Component"                        **********
// **********                                                               **********
// ***********************************************************************************
Component::Component()
{
  Type = isComponent;

  mirroredX = false;
  rotated = 0;
  isSelected = false;
  isActive   = true;

  cx = 0;
  cy = 0;

  Arcs.setAutoDelete(true);
  Lines.setAutoDelete(true);
  Ports.setAutoDelete(true);
  Texts.setAutoDelete(true);
  Props.setAutoDelete(true);
}

Component::~Component()
{
}

Component* Component::newOne()
{
  return new Component();
}

// -------------------------------------------------------
void Component::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = x1+cx;
  _y1 = y1+cy;
  _x2 = x2+cx;
  _y2 = y2+cy;
}

// -------------------------------------------------------
// Boundings including the component text.
void Component::entireBounds(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = x1+cx;
  _y1 = y1+cy;
  _x2 = x2+cx;
  _y2 = y2+cy;

  // text boundings
  if(tx < x1) _x1 = tx+cx;
  if(ty < y1) _y1 = ty+cy;

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",12, QFont::Light));
  QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Name);      // get width of text
  if((tx+r.width()) > x2) _x2 = tx+r.width()+cx;
  if((ty+r.height()) > y2) _y2 = ty+r.height()+cy;

  int dy=12; // due to 'Name' text
  for(Property *pp = Props.first(); pp != 0; pp = Props.next())
    if(pp->display) {
      r = p.boundingRect(0,0,0,0,Qt::AlignAuto,pp->Name+"="+pp->Value);      // get width of text
      if((tx+r.width()) > x2) _x2 = tx+r.width()+cx;
      dy += 12;
    }
  if((ty+dy) > y2) _y2 = ty+dy+cy;
}

// -------------------------------------------------------
void Component::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x;  cy = y; }
}

// -------------------------------------------------------
void Component::paint(QPainter *p)
{
  
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {     // paint all lines
    p->setPen(p1->style);
    p->drawLine(cx+p1->x1, cy+p1->y1, cx+p1->x2, cy+p1->y2);
  }

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // paint all arcs
    p->setPen(p3->style);
    p->drawArc(cx+p3->x, cy+p3->y, p3->w, p3->h, p3->angle, p3->arclen);
  }

  QFont tmp;
  if(Sign.at(0) == '.') {   // is simulation component (dc, ac, s parameter, ...)
    tmp = p->font();
    p->setFont(QFont("Helvetica",16, QFont::DemiBold));
    p->drawText(cx+x1+8, cy+y1+8, x2-x1, y2-y1, Qt::WordBreak, Description);
    p->setFont(tmp);
  }

  p->setPen(QPen(QPen::black,1));
  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next()) {     // write all text
    tmp = p->font();
    p->setFont(QFont("Helvetica",10, QFont::Light));
    p->drawText(cx+pt->x, cy+pt->y, pt->s);
    p->setFont(tmp);
  }

  int y=12;
  p->drawText(cx+tx, cy+ty, 0, 0, Qt::DontClip, Name);
  for(Property *p4 = Props.first(); p4 != 0; p4 = Props.next()) {    // write all properties
    if(p4->display) p->drawText(cx+tx, cy+ty+y, 0, 0, Qt::DontClip, p4->Name+"="+p4->Value);
    y += 12;
  }

  if(!isActive) {
    p->setPen(QPen(QPen::red,1));
    p->drawRect(cx+x1, cy+y1, x2-x1, y2-y1);
    p->drawLine(cx+x1, cy+y1, cx+x2, cy+y2);
    p->drawLine(cx+x1, cy+y2, cx+x2, cy+y1);
  }

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRoundRect(cx+x1, cy+y1, x2-x1, y2-y1);
  }
}

// -------------------------------------------------------
// Paints the component when moved with the mouse.
void Component::paintScheme(QPainter *p) //, int x, int y) //const QPoint& pos)
{
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next())     // paint all lines
    p->drawLine(cx+p1->x1, cy+p1->y1, cx+p1->x2, cy+p1->y2);

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next())     // paint all ports
    p->drawEllipse(cx+p2->x-4, cy+p2->y-4, 8, 8);
  
  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next())     // paint all arcs
    p->drawArc(cx+p3->x, cy+p3->y, p3->w, p3->h, p3->angle, p3->arclen);
}

// -------------------------------------------------------
// Rotates the component 90� counter-clockwise around its center
void Component::rotate()
{
  int tmp;
  
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {     // rotate all lines
    tmp = -p1->x1;
    p1->x1 = p1->y1;
    p1->y1 = tmp;
    tmp = -p1->x2;
    p1->x2 = p1->y2;
    p1->y2 = tmp;
  }

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next()) {     // rotate all ports
    tmp = -p2->x;
    p2->x = p2->y;
    p2->y = tmp;
  }

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // rotate all arcs
    tmp = -p3->x;
    p3->x = p3->y;
    p3->y = tmp - p3->w +1; // +1 is beauty correction
    tmp = p3->w;
    p3->w = p3->h;
    p3->h = tmp;
    p3->angle += 16*90;
    if(p3->angle >= 16*360) p3->angle -= 16*360;;
  }

  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next()) {     // rotate all text
    tmp = -pt->x; // - r.width();
    pt->x = pt->y - 8;
    pt->y = tmp;
  }

  tmp = -x1;   // rotate boundings
  x1  = y1; y1 = -x2;
  x2  = y2; y2 = tmp;
  
  tmp = tx;    // rotate text position
  tx  = ty;
  ty  = tmp;

  rotated++;  // keep track of what's done
  rotated &= 3;
}

// -------------------------------------------------------
// Mirrors the component about the x-axis.
void Component::mirrorX()
{
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {  // mirror all lines
    p1->y1 = -p1->y1;
    p1->y2 = -p1->y2;
  }

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next())  // mirror all ports
    p2->y = -p2->y;

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // mirror all arcs
    p3->y = -p3->y - p3->h + 1; // +1 is beauty correction
    if(p3->angle > 16*180) p3->angle -= 16*360;
    p3->angle  = -p3->angle;  // mirror
    p3->angle -= p3->arclen;  // go back to end of arc
    if(p3->angle < 0) p3->angle += 16*360;  // angle has to be > 0
  }

  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next())     // mirror all text
    pt->y = -pt->y + 10;

  int tmp = y1;
  y1  = -y2; y2 = -tmp;   // mirror boundings
  if(tx<0) ty = y2+4;   // mirror text position
  else ty = y1+4;

  mirroredX = !mirroredX;   // keep track of what's done
  rotated += rotated << 1;
  rotated &= 3;
}

// -------------------------------------------------------
// Mirrors the component about the y-axis.
void Component::mirrorY()
{
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {  // mirror all lines
    p1->x1 = -p1->x1;
    p1->x2 = -p1->x2;
  }

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next())  // mirror all ports
    p2->x = -p2->x;

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // mirror all arcs
    p3->x = -p3->x - p3->w + 1; // +1 is beauty correction
    p3->angle = 16*180 - p3->angle - p3->arclen;  // mirror
    if(p3->angle < 0) p3->angle += 16*360;   // angle has to be > 0
  }

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",10, QFont::Light));
  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next()) {     // mirror all text
    QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,pt->s);    // get width of text
    pt->x = -pt->x - r.width();
  }

  int tmp = x1;
  x1  = -x2; x2 = -tmp;   // mirror boundings
  if(ty<0) tx = x2+4;   // mirror text position
  else tx = x1+4;

  mirroredX = !mirroredX;   // keep track of what's done
  rotated += rotated << 1;
  rotated += 2;
  rotated &= 3;
}

// -------------------------------------------------------
QString Component::NetList()
{
  if(Model.isEmpty()) return QString("");   // dummy elements (e.g. ground)
  if(!isActive) return QString("");         // should it be simulated ?
  QString s = Model+":"+Name;
  
  
  for(Port *p1 = Ports.first(); p1 != 0; p1 = Ports.next())     // work on all ports
    s += " "+p1->Connection->Name;    // node names

  for(Property *p2 = Props.first(); p2 != 0; p2 = Props.next())    // work on all properties
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s;
}

// -------------------------------------------------------
QString Component::save()
{
  QString s = "   <"+Sign;
  if(Name.isEmpty()) s += " *";
  else s += " "+Name;

  if(isActive) s += " 1";
  else s += " 0";
  s += " "+QString::number(cx)+" "+QString::number(cy);
  s += " "+QString::number(tx)+" "+QString::number(ty);
  if(mirroredX) s += " 1";
  else s += " 0";
  s += " "+QString::number(rotated);
  
  for(Property *p1 = Props.first(); p1 != 0; p1 = Props.next()) {   // write all properties
    s += " \""+p1->Value+"\"";
    if(p1->display) s += " 1";
    else s += " 0";
  }

  return s+">";
}

// -------------------------------------------------------
bool Component::load(const QString& _s)
{
  bool ok;
  int  ttx, tty, tmp;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  QString n;
  Sign = s.section(' ',0,0);    // Sign

  Name = s.section(' ',1,1);      // Name
  if(Name == "*") Name = "";

  n  = s.section(' ',2,2);    // isActive
  if(n.toInt(&ok) == 1) isActive = true;
  else isActive = false;
  if(!ok) return false;

  n  = s.section(' ',3,3);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // tx
  ttx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // ty
  tty = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',7,7);    // mirroredX
  if(n.toInt(&ok) == 1) mirrorX();  // mirror component
  if(!ok) return false;

  n  = s.section(' ',8,8);    // rotated
  tmp = n.toInt(&ok);
  if(!ok) return false;
  for(int z=0; z<tmp; z++) rotate();   // rotate component

  tx = ttx; ty = tty;   // restore text position (was change by rotate/mirror)

  int z=0;
  for(Property *p1 = Props.first(); p1 != 0; p1 = Props.next()) {   // load all properties
    z++;
    p1->Value = s.section('"',z,z);    // property value
  
    z++;
    n  = s.section('"',z,z);    // display
    if(n.toInt(&ok) == 1) p1->display = true;
    else p1->display = false;
    if(!ok) return false;
  }
  
  return true;
}

// ******************************************************************************************
// **********                                                                      **********
// **********                        Data Components, etc.                         **********
// **********                                                                      **********
// ******************************************************************************************

SParamFile::SParamFile(int No)
{
  Description = "S parameter file";

  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-15, -h, 15, -h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 15, -h, 15,  h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15,  h, 15,  h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15, -h,-15,  h,QPen(QPen::darkBlue,2)));
  Texts.append(new Text( -6,  0,"file"));


  int i=0, y = 15-h;
  while(i<No) {
    i++;
    Lines.append(new Line(-30,  y,-15,  y,QPen(QPen::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    Texts.append(new Text(-25,y-3,QString::number(i)));

    if(i == No) break;
    i++;
    Lines.append(new Line( 15,  y, 30,  y,QPen(QPen::darkBlue,2)));
    Ports.append(new Port( 30,  y));
    Texts.append(new Text( 20,y-3,QString::number(i)));
    y += 60;
  }

  Lines.append(new Line( 0, h, 0,h+15,QPen(QPen::darkBlue,2)));
  Texts.append(new Text( 4,h+10,"Ref"));
  Ports.append(new Port( 0,h+15));    // 'Ref' port

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+15;

  tx = x1+4;
  ty = y2+4;
  Sign  = QString("SPfile")+QString::number(No);
  Model = QString("SPfile")+QString::number(No);
  Name  = "X";

  Props.append(new Property("File", "test.s2p", true, "name of the s parameter file"));
}

SParamFile::~SParamFile()
{
}

SParamFile* SParamFile::newOne()
{
  int z = Sign.mid(6).toInt();
  return new SParamFile(z);
}

// --------------------------------------------------------------------------
Subcircuit::Subcircuit(int No)
{
  Description = "subcircuit";

  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-15, -h, 15, -h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 15, -h, 15,  h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15,  h, 15,  h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15, -h,-15,  h,QPen(QPen::darkBlue,2)));
  Texts.append(new Text( -7,  0,"sub"));

  
  int i=0, y = 15-h;
  while(i<No) {
    i++;
    Lines.append(new Line(-30,  y,-15,  y,QPen(QPen::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    Texts.append(new Text(-25,y-3,QString::number(i)));

    if(i == No) break;
    i++;
    Lines.append(new Line( 15,  y, 30,  y,QPen(QPen::darkBlue,2)));
    Ports.append(new Port( 30,  y));
    Texts.append(new Text( 20,y-3,QString::number(i)));
    y += 60;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;

  tx = x1+4;
  ty = y2+4;
  Sign  = QString("Sub")+QString::number(No);
  Model = QString("Sub")+QString::number(No);
  Name  = "SUB";

  Props.append(new Property("File", "test.sch", true, "name of qucs schematic file"));
}

Subcircuit::~Subcircuit()
{
}

Subcircuit* Subcircuit::newOne()
{
  int z = Sign.mid(3).toInt();
  return new Subcircuit(z);
}

// --------------------------------------------------------------------------
SubCirPort::SubCirPort()
{
  Description = "port of a subcircuit";

  Arcs.append(new Arc(-25, -6, 13, 13,  0, 16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-14,  0,  0,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(  0,  0));

  x1 = -27; y1 = -8;
  x2 =   0; y2 =  8;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Port";
  Model = "";
  Name  = "P";

  Props.append(new Property("Num", "1", true));
}

SubCirPort::~SubCirPort()
{
}

SubCirPort* SubCirPort::newOne()
{
  return new SubCirPort();
}

// --------------------------------------------------------------------------
Equation::Equation()
{
  Description = "equation";

  Lines.append(new Line(-24, -6,-24,  6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-24,  6, 24,  6,QPen(QPen::darkBlue,2)));
  Texts.append(new Text(-20,  2,"Equation"));

  x1 = -27; y1 = -9;
  x2 =  27; y2 =  9;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Eqn";
  Model = "Eqn";
  Name  = "Eqn";

  Props.append(new Property("y", "1", true));
}

Equation::~Equation()
{
}

Equation* Equation::newOne()
{
  return new Equation();
}


// ******************************************************************************************
// **********                                                                      **********
// **********                            The Components                            **********
// **********                                                                      **********
// ******************************************************************************************

Resistor::Resistor()
{
  Description = "resistor";

  Lines.append(new Line(-18, -9, 18, -9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18, -9, 18,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  9,-18,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,  9,-18, -9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;

  tx = x1+4;
  ty = y2+4;
  Sign  = "R";
  Model = "R";
  Name  = "R";

  Props.append(new Property("R", "50 Ohm", true, "ohmic resistance in Ohms"));
//  Props.append(new Property("Noise", "yes", false));
}

Resistor::~Resistor()
{
}

Resistor* Resistor::newOne()
{
  return new Resistor();
}

// --------------------------------------------------------------------------
ResistorUS::ResistorUS()
{
  Description = "resistor (US symbol)";

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,  0,-15, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15, -7, -9,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -9,  7, -3, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -3, -7,  3,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  3,  7,  9, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  9, -7, 15,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 15,  7, 18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -9;
  x2 =  30; y2 =  9;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Rus";
  Model = "R";
  Name  = "R";

  Props.append(new Property("R", "50 Ohm", true, "ohmic resistance in Ohms"));
//  Props.append(new Property("Noise", "yes", false, "thermal noise yes or no"));
}

ResistorUS::~ResistorUS()
{
}

ResistorUS* ResistorUS::newOne()
{
  return new ResistorUS();
}

// --------------------------------------------------------------------------
Capacitor::Capacitor()
{
  Description = "capacitor";

  Lines.append(new Line( -4,-11, -4, 11,QPen(QPen::darkBlue,4)));
  Lines.append(new Line(  4,-11,  4, 11,QPen(QPen::darkBlue,4)));
  Lines.append(new Line(-30,  0, -4,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  4,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -13;
  x2 =  30; y2 =  13;

  tx = x1+4;
  ty = y2+4;
  Sign  = "C";
  Model = "C";
  Name  = "C";

  Props.append(new Property("C", "1 pF", true, "capacitance in Farad"));
}

Capacitor::~Capacitor()
{
}

Capacitor* Capacitor::newOne()
{
  return new Capacitor();
}

// --------------------------------------------------------------------------
dcBlock::dcBlock()
{
  Description = "dc block";

  Lines.append(new Line(- 4,-11, -4, 11,QPen(QPen::darkBlue,4)));
  Lines.append(new Line(  4,-11,  4, 11,QPen(QPen::darkBlue,4)));
  Lines.append(new Line(-30,  0, -4,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  4,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-23,-14, 23,-14,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-23, 14, 23, 14,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-23,-14,-23, 14,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 23,-14, 23, 14,QPen(QPen::darkBlue,1)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Sign  = "DCblock";
  Model = "DCblock";
  Name  = "C";
}

dcBlock::~dcBlock()
{
}

dcBlock* dcBlock::newOne()
{
  return new dcBlock();
}

// --------------------------------------------------------------------------
Inductor::Inductor()
{
  Description = "inductor";

  Arcs.append(new Arc(-17, -6, 13, 13,  0, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( -6, -6, 13, 13,  0, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  5, -6, 13, 13,  0, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-17,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 17,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -10;
  x2 =  30; y2 =   6;

  tx = x1+4;
  ty = y2+4;
  Sign  = "L";
  Model = "L";
  Name  = "L";

  Props.append(new Property("L", "1 nH", true, "inductance in Henry"));
}

Inductor::~Inductor()
{
}

Inductor* Inductor::newOne()
{
  return new Inductor();
}

// --------------------------------------------------------------------------
dcFeed::dcFeed()
{
  Description = "dc feed";

  Arcs.append(new Arc(-17, -6, 13, 13,  0, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( -6, -6, 13, 13,  0, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  5, -6, 13, 13,  0, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-17,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 17,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-23,-13, 23,-13,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-23, 13, 23, 13,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-23,-13,-23, 13,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 23,-13, 23, 13,QPen(QPen::darkBlue,1)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -13;
  x2 =  30; y2 =  13;

  tx = x1+4;
  ty = y2+4;
  Sign  = "DCfeed";
  Model = "dcfeed";
  Name  = "L";
}

dcFeed::~dcFeed()
{
}

dcFeed* dcFeed::newOne()
{
  return new dcFeed();
}

// --------------------------------------------------------------------------
BiasT::BiasT()
{
  Description = "bias t";

  Arcs.append(new Arc( -3,  2, 7, 7, 16*270, 16*180,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc( -3,  8, 7, 7, 16*270, 16*180,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc( -3, 14, 7, 7, 16*270, 16*180,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-22,-10, 22,-10,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-22,-10,-22, 22,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-22, 22, 22, 22,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 22,-10, 22, 22,QPen(QPen::darkBlue,1)));

  Lines.append(new Line(-13, -6,-13,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -9, -6, -9,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -9,  0, 22,  0,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-22,  0,-13,  0,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-30,  0,-22,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 22,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  0,  0,  2,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(  0, 20,  0, 22,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(  0, 22,  0, 30,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = "BiasT";
  Model = "BiasT";
  Name  = "X";
}

BiasT::~BiasT()
{
}

BiasT* BiasT::newOne()
{
  return new BiasT();
}

// --------------------------------------------------------------------------
Transformer::Transformer()
{
  Description = "ideal transformer";

  Arcs.append(new Arc(-16,-18, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, -6, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16,  5, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,-18, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4, -6, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,  5, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-18,-10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-30,-30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-18, 10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 17,-10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 30,-30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 17, 10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 30, 30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -1,-20, -1, 20,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(  1,-20,  1, 20,QPen(QPen::darkBlue,1)));

  Texts.append(new Text(-21, -8,"T"));
  Arcs.append(new Arc(-21,-24,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 15,-24,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
  

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -33; y1 = -33;
  x2 =  33; y2 =  33;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Tr";
  Model = "Tr";
  Name  = "Tr";

  Props.append(new Property("T", "1", true, "voltage transformation ration"));
}

Transformer::~Transformer()
{
}

Transformer* Transformer::newOne()
{
  return new Transformer();
}

// --------------------------------------------------------------------------
symTrafo::symTrafo()
{
  Description = "ideal symmetrical transformer";

  Arcs.append(new Arc(-16,-58, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16,-46, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16,-34, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, 46, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, 34, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, 22, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,-18, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4, -6, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,  5, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-58,-10,-70,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-70,-30,-70,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-18, 10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 58,-10, 70,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 70,-30, 70,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 17, 10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 30, 30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-10,-30,-10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-22,-10,-10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 10,-30, 10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 10,-10, 22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -1,-57, -1, 57,QPen(QPen::darkBlue,1))); // core lines
  Lines.append(new Line(  1,-57,  1, 57,QPen(QPen::darkBlue,1)));

  Texts.append(new Text(-23,-47,"T1"));
  Texts.append(new Text(-23, 32,"T2"));
  
  Arcs.append(new Arc(-21,-64,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2))); // mark the turn direction
  Arcs.append(new Arc(-21, 15,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 15,-24,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
      
  Ports.append(new Port(-30,-70));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 70));
  Ports.append(new Port(-30, 10));
  Ports.append(new Port(-30,-10));

  x1 = -33; y1 = -73;
  x2 =  33; y2 =  73;

  tx = x1+4;
  ty = y2+4;
  Sign  = "sTr";
  Model = "sTr";
  Name  = "Tr";

  Props.append(new Property("T1", "1", true, "voltage transformation ration of coil 1"));
  Props.append(new Property("T2", "1", true, "voltage transformation ration of coil 2"));
}

symTrafo::~symTrafo()
{
}

symTrafo* symTrafo::newOne()
{
  return new symTrafo();
}

// --------------------------------------------------------------------------
Ground::Ground()
{
  Description = "ground (reference potential)";

  Lines.append(new Line(  0,  0,  0, 10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-11, 10, 11, 10,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( -7, 16,  7, 16,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( -3, 22,  3, 22,QPen(QPen::darkBlue,3)));

  Ports.append(new Port(  0,  0));

  x1 = -12; y1 =  0;
  x2 =  12; y2 = 25;

  tx = 0;
  ty = 0;
  Sign  = "GND";
  Model = "";
  Name  = "";
}

Ground::~Ground()
{
}

Ground* Ground::newOne()
{
  return new Ground();
}

// --------------------------------------------------------------------------
Attenuator::Attenuator()
{
  Description = "attenuator";

  Lines.append(new Line( -4, -6, -4,  6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -4, -6,  4, -6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  4, -6,  4,  6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -4,  6,  4,  6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-11,  0, -6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  6,  0, 11,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-14,-14, 14,-14,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-14, 14, 14, 14,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-14,-14,-14, 14,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 14,-14, 14, 14,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30,  0,-14,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 14,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -15;
  x2 =  30; y2 =  15;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Attenuator";
  Model = "Attenuator";
  Name  = "X";

  Props.append(new Property("L", "10 dB", true, "attenuation"));
}

Attenuator::~Attenuator()
{
}

Attenuator* Attenuator::newOne()
{
  return new Attenuator();
}

// --------------------------------------------------------------------------
Isolator::Isolator()
{
  Description = "isolator";

  Lines.append(new Line( -8,  0,  8,  0,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(  8,  0,  0, -5,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(  8,  0,  0,  5,QPen(QPen::darkBlue,3)));

  Lines.append(new Line(-14,-14, 14,-14,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-14, 14, 14, 14,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-14,-14,-14, 14,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 14,-14, 14, 14,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30,  0,-14,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 14,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -15;
  x2 =  30; y2 =  15;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Isolator";
  Model = "Isolator";
  Name  = "X";
}

Isolator::~Isolator()
{
}

Isolator* Isolator::newOne()
{
  return new Isolator();
}

// --------------------------------------------------------------------------
Circulator::Circulator()
{
  Description = "circulator";

  Arcs.append(new Arc(-14,-14, 29, 29,  0,16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-14,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 14,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, 14,  0, 30,QPen(QPen::darkBlue,2)));

  Arcs.append(new Arc( -8, -6, 17, 17,16*20,16*150,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8,  0,  9, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8,  0,  2, -1,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Circulator";
  Model = "Circulator";
  Name  = "X";
}

Circulator::~Circulator()
{
}

Circulator* Circulator::newOne()
{
  return new Circulator();
}

// --------------------------------------------------------------------------
Volt_dc::Volt_dc()
{
  Description = "ideal dc voltage source";

  Lines.append(new Line(  4,-13,  4, 13,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -4, -6, -4,  6,QPen(QPen::darkBlue,4)));
  Lines.append(new Line( 30,  0,  4,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -4,  0,-30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,  5, 10, 11,QPen(QPen::red,1)));
  Lines.append(new Line( 13,  8,  7,  8,QPen(QPen::red,1)));
  Lines.append(new Line( -9,  5, -9, 11,QPen(QPen::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Sign  = "V";
  Model = "V";
  Name  = "V";

  Props.append(new Property("U", "1 V", true, "voltage in Volts"));
}

Volt_dc::~Volt_dc()
{
}

Volt_dc* Volt_dc::newOne()
{
  return new Volt_dc();
}

// --------------------------------------------------------------------------
Ampere_dc::Ampere_dc()
{
  Description = "ideal dc current source";

  Arcs.append(new Arc(-12,-12, 25, 25,  0, 16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(QPen::darkBlue,3)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Sign  = "I";
  Model = "I";
  Name  = "I";

  Props.append(new Property("I", "1 mA", true, "current in Ampere"));
}

Ampere_dc::~Ampere_dc()
{
}

Ampere_dc* Ampere_dc::newOne()
{
  return new Ampere_dc();
}

// --------------------------------------------------------------------------
Volt_ac::Volt_ac()
{
  Description = "ideal ac voltage source";

  Arcs.append(new Arc(-12,-12, 25, 25,     0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( -4, -8,  9,  9,16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( -3,  0,  9,  9, 16*90, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  5, 18, 11,QPen(QPen::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(QPen::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(QPen::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Vac";
  Model = "Vac";
  Name  = "V";

  Props.append(new Property("U", "1 V", true, "rms voltage in Volts"));
  Props.append(new Property("f", "1 GHz", true, "frequency in Hertz"));
}

Volt_ac::~Volt_ac()
{
}

Volt_ac* Volt_ac::newOne()
{
  return new Volt_ac();
}

// --------------------------------------------------------------------------
Source_ac::Source_ac()
{
  Description = "ac power source";

  Lines.append(new Line(-22,-11, 22,-11,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-22, 10, 22, 10,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-22,-11,-22, 10,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 22,-11, 22, 10,QPen(QPen::darkGray,1)));

  Arcs.append(new Arc(-20, -9, 18, 18,     0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-15, -6,  7,  7,16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-14, -1,  7,  7, 16*90, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-20,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 19,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -3,  0,  3,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(  3, -5, 19, -5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  3,  5, 19,  5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  3, -5,  3,  5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 19, -5, 19,  5,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( 25,  5, 25, 11,QPen(QPen::red,1)));
  Lines.append(new Line( 28,  8, 22,  8,QPen(QPen::red,1)));
  Lines.append(new Line(-25,  5,-25, 11,QPen(QPen::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Pac";
  Model = "Pac";
  Name  = "P";

  Props.append(new Property("Num", "1", true, "number of the port"));
  Props.append(new Property("Z", "50 Ohm", true, "port impedance"));
  Props.append(new Property("P", "0 dBm", true, "ac power in Watts"));
  Props.append(new Property("f", "1 GHz", true, "frequency in Hertz"));
}

Source_ac::~Source_ac()
{
}

Source_ac* Source_ac::newOne()
{
  return new Source_ac();
}

// --------------------------------------------------------------------------
VCCS::VCCS()
{
  Description = "voltage controlled current source";

  Arcs.append(new Arc(0,-11, 23, 23,  0, 16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, -7, 11,  7,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( 11,  6, 15,  1,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( 11,  6,  7,  1,QPen(QPen::darkBlue,3)));

  Lines.append(new Line(-30,-30,-12,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 30,-12, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, 30, 30, 30,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-12,-30,-12,-23,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 30,-12, 23,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11,-30, 11,-11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, 30, 11, 11,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-12,-18,-12, 18,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 18,-17,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 18, -8,  9,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-25,-27, 25,-27,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 25,-27, 25, 27,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 25, 27,-25, 27,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-25, 27,-25,-27,QPen(QPen::darkGray,1)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = "VCCS";
  Model = "VCCS";
  Name  = "SRC";

  Props.append(new Property("G", "1 S", true, "forward transconductance"));
  Props.append(new Property("T", "0", true, "delay time"));
}

VCCS::~VCCS()
{
}

VCCS* VCCS::newOne()
{
  return new VCCS();
}

// --------------------------------------------------------------------------
CCCS::CCCS()
{
  Description = "current controlled current source";

  Arcs.append(new Arc(0,-11, 23, 23,  0, 16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, -7, 11,  7,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( 11,  6, 15,  1,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( 11,  6,  7,  1,QPen(QPen::darkBlue,3)));

  Lines.append(new Line(-30,-30,-12,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 30,-12, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, 30, 30, 30,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-12,-30,-12, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11,-30, 11,-11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, 30, 11, 11,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-12, 20,-17, 11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 20, -8, 11,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-25,-27, 25,-27,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 25,-27, 25, 27,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 25, 27,-25, 27,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-25, 27,-25,-27,QPen(QPen::darkGray,1)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = "CCCS";
  Model = "CCCS";
  Name  = "SRC";

  Props.append(new Property("G", "1", true, "forward transfer factor"));
  Props.append(new Property("T", "0", true, "delay time"));
}

CCCS::~CCCS()
{
}

CCCS* CCCS::newOne()
{
  return new CCCS();
}

// --------------------------------------------------------------------------
VCVS::VCVS()
{
  Description = "voltage controlled voltage source";

  Arcs.append(new Arc(0,-11, 23, 23,  0, 16*360,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30,-30,-12,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 30,-12, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, 30, 30, 30,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-12,-30,-12,-23,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 30,-12, 23,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11,-30, 11,-11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, 30, 11, 11,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-12,-18,-12, 18,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 18,-17,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 18, -8,  9,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( 19,-21, 19,-15,QPen(QPen::red,1)));
  Lines.append(new Line( 16,-18, 22,-18,QPen(QPen::red,1)));
  Lines.append(new Line( 16, 18, 22, 18,QPen(QPen::black,1)));

  Lines.append(new Line(-25,-27, 25,-27,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 25,-27, 25, 27,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 25, 27,-25, 27,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-25, 27,-25,-27,QPen(QPen::darkGray,1)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = "VCVS";
  Model = "VCVS";
  Name  = "SRC";

  Props.append(new Property("G", "1", true, "forward transfer factor"));
  Props.append(new Property("T", "0", true, "delay time"));
}

VCVS::~VCVS()
{
}

VCVS* VCVS::newOne()
{
  return new VCVS();
}

// --------------------------------------------------------------------------
CCVS::CCVS()
{
  Description = "current controlled voltage source";

  Arcs.append(new Arc(0,-11, 23, 23,  0, 16*360,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30,-30,-12,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 30,-12, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, 30, 30, 30,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-12,-30,-12, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11,-30, 11,-11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, 30, 11, 11,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-12, 20,-17, 11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 20, -8, 11,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( 19,-21, 19,-15,QPen(QPen::red,1)));
  Lines.append(new Line( 16,-18, 22,-18,QPen(QPen::red,1)));
  Lines.append(new Line( 16, 18, 22, 18,QPen(QPen::black,1)));

  Lines.append(new Line(-25,-27, 25,-27,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 25,-27, 25, 27,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 25, 27,-25, 27,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-25, 27,-25,-27,QPen(QPen::darkGray,1)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = "CCVS";
  Model = "CCVS";
  Name  = "SRC";

  Props.append(new Property("G", "1 Ohm", true, "forward transfer factor"));
  Props.append(new Property("T", "0", true, "delay time"));
}

CCVS::~CCVS()
{
}

CCVS* CCVS::newOne()
{
  return new CCVS();
}


// ******************************************************************************************
// **********                                                                      **********
// **********                         Transmission Lines                           **********
// **********                                                                      **********
// ******************************************************************************************

TLine::TLine()
{
  Description = "ideal transmission line";

  Arcs.append(new Arc(-20, -9, 9, 19,     0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 11, -9, 9, 19,16*270, 16*180,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30,  0,-16,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-16, -9, 16, -9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-16,  9, 16,  9,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-10;
  x2 =  30; y2 = 10;

  tx = x1+4;
  ty = y2+4;
  Sign  = "TLIN";
  Model = "TLIN";
  Name  = "Line";

  Props.append(new Property("Z", "50 Ohm", true, "characteristic impedance"));
  Props.append(new Property("L", "1 mm", true, "electrical length of the line"));
}

TLine::~TLine()
{
}

TLine* TLine::newOne()
{
  return new TLine();
}

// ------------------------------------------------------------------------
Substrate::Substrate()
{
  Description = "substrate definition";

  Lines.append(new Line(-30,-16, 30,-16,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,-12, 30,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 16, 30, 16,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 12, 30, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,-16,-30, 16,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,-16, 30, 16,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30,-16, 16,-40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,-16, 80,-40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,-12, 80,-36,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 12, 80,-16,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 16, 80,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 16,-40, 80,-40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 80,-40, 80,-12,QPen(QPen::darkBlue,2)));
  
  Lines.append(new Line(-30,  0,-18,-12,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-22, 12,  2,-12,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( -2, 12, 22,-12,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 18, 12, 30,  0,QPen(QPen::darkBlue,1)));

  Lines.append(new Line( 30,  1, 37,  8,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 37,-15, 52,  0,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 52,-22, 66, -8,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 66,-30, 80,-16,QPen(QPen::darkBlue,1)));

  x1 = -30; y1 =-40;
  x2 =  80; y2 = 16;

  tx = x1+4;
  ty = y2+4;
  Sign  = "SUBST";
  Model = "SUBST";
  Name  = "Subst";

  Props.append(new Property("er", "9.8", true, "permittivity"));
  Props.append(new Property("h", "1 mm", true, "thickness in meters"));
  Props.append(new Property("t", "35 um", true, "thickness of metalization"));
  Props.append(new Property("tand", "0", true, "loss tangent"));
}

Substrate::~Substrate()
{
}

Substrate* Substrate::newOne()
{
  return new Substrate();
}

// ------------------------------------------------------------------------
MSline::MSline()
{
  Description = "microstrip line";

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-13, -8, 23, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-23,  8, 13,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-13, -8,-23,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 23, -8, 13,  8,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-22, -4,-26,  4,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-11;
  x2 =  30; y2 = 11;

  tx = x1+4;
  ty = y2+4;
  Sign  = "MLIN";
  Model = "MLIN";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true, "name of substrate definition"));
  Props.append(new Property("W", "1 mm", true, "width of the line"));
  Props.append(new Property("L", "10 mm", true, "length of the line"));
  Props.append(new Property("Model", "Kirschning", false, "microstrip model |Kirschning|Kobayashi|Yamashita"));
}

MSline::~MSline()
{
}

MSline* MSline::newOne()
{
  return new MSline();
}

// ------------------------------------------------------------------------
MScoupled::MScoupled()
{
  Description = "coupled microstrip line";

  Lines.append(new Line(-30,-12,-16,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,-30,-30,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 20,-12, 30,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,-30, 30,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-11,-20, 25,-20,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-21, -4, 15, -4,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-11,-20,-21, -4,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 25,-20, 15, -4,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30, 12,-20, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 30,-30, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 16, 12, 30, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 30, 30, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15,  4, 21,  4,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-25, 20, 11, 20,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15,  4,-25, 20,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 21,  4, 11, 20,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-22,-16,-26, -8,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -30; y1 =-33;
  x2 =  30; y2 = 33;

  tx = x1+4;
  ty = y2+4;
  Sign  = "CMS";
  Model = "CMS";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true, "name of substrate definition"));
  Props.append(new Property("W", "1 mm", true, "width of the line"));
  Props.append(new Property("L", "10 mm", true, "length of the line"));
  Props.append(new Property("S", "1 mm", true, "spacing between the lines"));
  Props.append(new Property("Model", "Kirschning", false, "microstrip model |Kirschning|Kobayashi|Yamashita"));
}

MScoupled::~MScoupled()
{
}

MScoupled* MScoupled::newOne()
{
  return new MScoupled();
}

// ------------------------------------------------------------------------
MSstep::MSstep()
{
  Description = "microstrip impedance step";

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,-12,  0,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18, 12,  0, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,-12,-18, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, -7, 18, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  7, 18,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18, -7, 18,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-12,  0, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  7,  0, 12,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-22, -4,-26,  4,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-13;
  x2 =  30; y2 = 13;

  tx = x1+4;
  ty = y2+4;
  Sign  = "MSTEP";
  Model = "MSTEP";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true, "substrate"));
  Props.append(new Property("W1", "2 mm", true, "width 1 of the line"));
  Props.append(new Property("W2", "1 mm", true, "width 2 of the line"));
}

MSstep::~MSstep()
{
}

MSstep* MSstep::newOne()
{
  return new MSstep();
}

// ------------------------------------------------------------------------
MScorner::MScorner()
{
  Description = "microstrip corner";

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, 18,  0, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18, -8,  8, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,  8, -8,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18, -8,-18,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -8,  8, -8, 18,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8, -8,  8, 18,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -8, 18,  8, 18,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-22, -4,-26,  4,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port(  0,30));

  x1 = -30; y1 = -9;
  x2 =   9; y2 = 30;

  tx = x1+4;
  ty = y2+4;
  Sign  = "MCORN";
  Model = "MCORN";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true, "substrate"));
  Props.append(new Property("W1", "1 mm", true, "width of line 1"));
  Props.append(new Property("W2", "2 mm", true, "width of line 2"));
}

MScorner::~MScorner()
{
}

MScorner* MScorner::newOne()
{
  return new MScorner();
}

// ------------------------------------------------------------------------
MStee::MStee()
{
  Description = "microstrip tee";

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, 18,  0, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18, -8, 18, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,  8, -8,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8,  8, 18,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18, -8,-18,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18, -8, 18,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -8,  8, -8, 18,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8,  8,  8, 18,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -8, 18,  8, 18,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-22, -4,-26,  4,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));
  Ports.append(new Port(  0,30));

  x1 = -30; y1 = -9;
  x2 =  30; y2 = 30;

  tx = x1+4;
  ty = y2+4;
  Sign  = "MTEE";
  Model = "MTEE";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true, "substrate"));
  Props.append(new Property("W1", "1 mm", true, "width of line 1"));
  Props.append(new Property("W2", "1 mm", true, "width of line 2"));
  Props.append(new Property("W3", "2 mm", true, "width of line 3"));
}

MStee::~MStee()
{
}

MStee* MStee::newOne()
{
  return new MStee();
}

// ------------------------------------------------------------------------
MScross::MScross()
{
  Description = "microstrip cross";

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, 18,  0, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-30,  0,-18,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-18, -8, -8, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,  8, -8,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18, -8,-18,  8,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(  8, -8, 18, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8,  8, 18,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18, -8, 18,  8,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( -8,  8, -8, 18,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8,  8,  8, 18,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -8, 18,  8, 18,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( -8,-18, -8, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8,-18,  8, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -8,-18,  8,-18,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-22, -4,-26,  4,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 =-30;
  x2 =  30; y2 = 30;

  tx = x1+4;
  ty = y2+4;
  Sign  = "MCROSS";
  Model = "MCROSS";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true, "substrate"));
  Props.append(new Property("W1", "1 mm", true, "width of line 1"));
  Props.append(new Property("W2", "2 mm", true, "width of line 2"));
  Props.append(new Property("W3", "1 mm", true, "width of line 3"));
  Props.append(new Property("W4", "2 mm", true, "width of line 4"));
}

MScross::~MScross()
{
}

MScross* MScross::newOne()
{
  return new MScross();
}


// ------------------------------------------------------------------------
Coplanar::Coplanar()
{
  Description = "coplanar line";

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-13, -8, 23, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-23,  8, 13,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-13, -8,-23,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 23, -8, 13,  8,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-25,-13, 25,-13,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 16,-21, 24,-13,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8,-21, 16,-13,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-21,  8,-13,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -8,-21,  0,-13,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-16,-21, -8,-13,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-24,-21,-16,-13,QPen(QPen::darkBlue,2)));
  
  Lines.append(new Line(-25, 13, 25, 13,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-24, 13,-16, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-16, 13, -8, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -8, 13,  0, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, 13,  8, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8, 13, 16, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 16, 13, 24, 21,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-24;
  x2 =  30; y2 = 24;

  tx = x1+4;
  ty = y2+4;
  Sign  = "CLIN";
  Model = "CLIN";
  Name  = "CL";

  Props.append(new Property("Subst", "Subst1", true, "name of substrate definition"));
  Props.append(new Property("W", "1 mm", true, "width of the line"));
  Props.append(new Property("S", "1 mm", true, "width of a gap"));
  Props.append(new Property("L", "10 mm", true, "length of the line"));
//  Props.append(new Property("Model", "Kirschning", false, "microstrip model |Kirschning|Kobayashi|Yamashita"));
}

Coplanar::~Coplanar()
{
}

Coplanar* Coplanar::newOne()
{
  return new Coplanar();
}

// ******************************************************************************************
// **********                                                                      **********
// **********                        Nonlinear Components                          **********
// **********                                                                      **********
// ******************************************************************************************

Diode::Diode()
{
  Description = "diode";

  Lines.append(new Line(-30,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -6, -9, -6,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  6, -9,  6,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -6,  0,  6, -9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -6,  0,  6,  9,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 = -9;
  x2 =  30; y2 =  9;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Diode";
  Model = "Diode";
  Name  = "D";
}

Diode::~Diode()
{
}

Diode* Diode::newOne()
{
  return new Diode();
}


// ******************************************************************************************
// **********                                                                      **********
// **********                         The Simulation Sets                          **********
// **********                                                                      **********
// ******************************************************************************************

DC_Sim::DC_Sim()
{
  Description = "dc simulation";

  Lines.append(new Line(-56,-12, 56,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56, 12, 56, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56,-12,-56, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56,-12, 56, 12,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-51, 17, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56, 12,-51, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56, 12, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 60, -7, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56,-12, 60, -7,QPen(QPen::darkBlue,2)));

  x1 = -60; y1 = -16;
  x2 =  64; y2 =  21;

  tx = x1+4;
  ty = y2+4;
  Sign  = ".DC";
  Model = ".DC";
  Name  = "DC";
}

DC_Sim::~DC_Sim()
{
}

DC_Sim* DC_Sim::newOne()
{
  return new DC_Sim();
}

// --------------------------------------------------------------------------
AC_Sim::AC_Sim()
{
  Description = "ac simulation";

  Lines.append(new Line(-56,-12, 56,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56, 12, 56, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56,-12,-56, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56,-12, 56, 12,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-51, 17, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-56, 12,-51, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56, 12, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 60, -7, 60, 17,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 56,-12, 60, -7,QPen(QPen::darkBlue,2)));

  x1 = -60; y1 = -16;
  x2 =  64; y2 =  21;

  tx = x1+4;
  ty = y2+4;
  Sign  = ".AC";
  Model = ".AC";
  Name  = "AC";

  Props.append(new Property("Start", "1 GHz", true, "start frequency in Hertz"));
  Props.append(new Property("Stop", "10 GHz", true, "stop frequency in Hertz"));
  Props.append(new Property("Step", "1 GHz", true, "frequency steps in Hertz"));
}

AC_Sim::~AC_Sim()
{
}

AC_Sim* AC_Sim::newOne()
{
  return new AC_Sim();
}

// --------------------------------------------------------------------------
TR_Sim::TR_Sim()
{
  Description = "transient simulation";

  Lines.append(new Line(-45,-21, 45,-21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45, 21, 45, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45,-21,-45, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 45,-21, 45, 21,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-41, 26, 49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45, 21,-41, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 45, 21, 49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 49,-16, 49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 45,-21, 49,-16,QPen(QPen::darkBlue,2)));

  x1 = -49; y1 = -25;
  x2 =  53; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = ".TR";
  Model = ".TR";
  Name  = "TR";

  Props.append(new Property("Start", "0", true, "start time in seconds"));
  Props.append(new Property("Stop", "1 ms", true, "stop time in seconds"));
  Props.append(new Property("Step", "10 us", true, "time step in seconds"));
}

TR_Sim::~TR_Sim()
{
}

TR_Sim* TR_Sim::newOne()
{
  return new TR_Sim();
}

// --------------------------------------------------------------------------
SP_Sim::SP_Sim()
{
  Description = "S parameter simulation";

  Lines.append(new Line(-53,-21, 53,-21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-53, 21, 53, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-53,-21,-53, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 53,-21, 53, 21,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-49, 26, 57, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-53, 21,-49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 53, 21, 57, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 57,-16, 57, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 53,-21, 57,-16,QPen(QPen::darkBlue,2)));

  x1 = -57; y1 = -25;
  x2 =  61; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = ".SP";
  Model = ".SP";
  Name  = "SP";

  Props.append(new Property("Start", "1 GHz", true, "start frequency in Hertz"));
  Props.append(new Property("Stop", "10 GHz", true, "stop frequency in Hertz"));
  Props.append(new Property("Step", "1 GHz", true, "frequency steps in Hertz"));
}

SP_Sim::~SP_Sim()
{
}

SP_Sim* SP_Sim::newOne()
{
  return new SP_Sim();
}

// --------------------------------------------------------------------------
HB_Sim::HB_Sim()
{
  Description = "Harmonic balance simulation";

  Lines.append(new Line(-75,-21, 75,-21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-75, 21, 75, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-75,-21,-75, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 75,-21, 75, 21,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-71, 26, 79, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-75, 21,-71, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 75, 21, 79, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 79,-16, 79, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 75,-21, 79,-16,QPen(QPen::darkBlue,2)));

  x1 = -79; y1 = -25;
  x2 =  83; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = ".HB";
  Model = ".HB";
  Name  = "HB";

  Props.append(new Property("f", "1 GHz", true, "frequency in Hertz"));
  Props.append(new Property("n", "4", true, "number of harmonics"));
}

HB_Sim::~HB_Sim()
{
}

HB_Sim* HB_Sim::newOne()
{
  return new HB_Sim();
}

// --------------------------------------------------------------------------
Param_Sweep::Param_Sweep()
{
  Description = "Parameter sweep";

  Lines.append(new Line(-46,-22, 46,-22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-46, 22, 46, 22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-46,-22,-46, 22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 46,-22, 46, 22,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-42, 27, 50, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-46, 22,-42, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 46, 22, 50, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 50,-17, 50, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 46,-22, 50,-17,QPen(QPen::darkBlue,2)));

  x1 = -50; y1 = -26;
  x2 =  54; y2 =  31;

  tx = x1+4;
  ty = y2+4;
  Sign  = ".SW";
  Model = ".SW";
  Name  = "SW";

  Props.append(new Property("Param", "R1", true, "parameter to sweep"));
  Props.append(new Property("Start", "5 Ohm", true, "start value for sweep"));
  Props.append(new Property("Stop", "50 Ohm", true, "stop value for sweep"));
  Props.append(new Property("Step", "5 Ohm", true, "step size for sweep"));
}

Param_Sweep::~Param_Sweep()
{
}

Param_Sweep* Param_Sweep::newOne()
{
  return new Param_Sweep();
}
