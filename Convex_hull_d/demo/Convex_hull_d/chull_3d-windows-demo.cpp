// Copyright (c) 2001, 2002, 2004  Max-Planck-Institute Saarbruecken (Germany).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Michael Seel <seel@mpi-sb.mpg.de>

#include <CGAL/basic.h>

#ifdef CGAL_USE_LEDA
#include <CGAL/LEDA_basic.h>
#include <CGAL/leda_integer.h>
#include <CGAL/Convex_hull_d.h>
#include <CGAL/IO/Convex_hull_d_window_stream.h>
#include <iostream>

#if CGAL_LEDA_VERSION < 500
#include <LEDA/random_source.h>
#include <LEDA/d3_window.h>
#else
#include <LEDA/core/random_source.h>
#include <LEDA/graphics/d3_window.h>
#endif

typedef leda_integer RT;

#define DDKERNEL
#ifdef DDKERNEL
#include <CGAL/Homogeneous_d.h>
typedef CGAL::Homogeneous_d<RT> Kernel_d;
typedef CGAL::Convex_hull_d<Kernel_d> Convex_hull_d;
#else
#include <CGAL/Homogeneous.h>
#include <CGAL/Convex_hull_d_traits_3.h>
typedef CGAL::Homogeneous<RT> Kernel_3;
typedef CGAL::Convex_hull_d_traits_3<Kernel_3> Kernel_d_3;
typedef CGAL::Convex_hull_d<Kernel_d_3> Convex_hull_d;
#endif
typedef Convex_hull_d::Point_d Point;
typedef Convex_hull_d::Simplex_handle Simplex_handle;

leda_vector leda_vec(const Point& p)
{ return leda_vector(CGAL::to_double(p.x()),
                     CGAL::to_double(p.y()),
                     CGAL::to_double(p.z()));
}


int main(int argc, char* argv[]) {
  CGAL::set_pretty_mode ( std::cerr );
  SETDTHREAD(93);
  int numpoints = 20;
  leda_string ifile("");
  if (argc > 1) numpoints = atoi(argv[1]);
  if (numpoints == 0 && argc > 2) ifile = leda_string(argv[2]);
  leda_string startmess = "we randomly input ";
  startmess += leda_string("%i",numpoints);
  startmess += " and display the convex hull in space!";
  leda_window W("Convex Hulls in Space"); 
  enum { SURFACE, SKELETON, EXIT };
  W.button("surface", SURFACE);
  W.button("skeleton", SKELETON);
  W.button("exit", EXIT);
  W.init(-50,50,-50);
  W.display(); 
  W.message(startmess);

  Convex_hull_d T(3);  
  leda_list<Point> L;
  std::ifstream from(ifile);
  if (from) from >> L;
  else {
    int r = numpoints;
    CGAL_LEDA_SCOPE::rand_int.set_range(-r,r);
    int x,y,z;
    while (r--) { 
      CGAL_LEDA_SCOPE::rand_int >> x >> y >> z; 
      L.append(Point(x,y,z,1));
    }
  }

  std::ofstream To("ch3-demo.log");
  Point p;
  // forall(p,L) { To << p; T.insert(p); T.is_valid(true); }
  T.initialize(L.begin(),L.end());
  T.print_statistics();
  To.flush(); 
  CGAL_LEDA_SCOPE::GRAPH<Point,int> G;
  leda_node_array<leda_vector> pos(G);
  leda_node v;
  leda_d3_window W3(W,G,pos);
  int but = SURFACE;
  while (but != EXIT) {
    switch (but) {
    case SKELETON:
      W3.set_solid(false);
      W3.set_elim(false);
      W3.set_node_width(2);
      W3.set_node_color(leda_red);
      CGAL::d3_graph(T,G);
      break;
    case SURFACE:
    default: // SURFACE
      W3.set_solid(true);
      W3.set_elim(true);
      CGAL::d3_surface_map(T,G);
      break;
    }
    pos.init(G);
    forall_nodes(v,G) pos[v] = leda_vec(G[v]);
    W3.init(pos);
    W3.draw();
    while (but != MOUSE_BUTTON(3))
      but = W3.move();
    but = W.read_mouse();
  }
}

#else
#include <iostream>

int main()
{ 
  std::cout << "this program requires LEDA" << std::endl;
  return 0;
}

#endif // CGAL_USE_LEDA

